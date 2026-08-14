#include "D3D12Device.h"
#include "RHI/Definitions.h"

#if RHI_D3D12

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

extern "C" 
{
	__declspec(dllexport) extern const UINT D3D12SDKVersion = 720;
	__declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\";
}
namespace Alpha
{
	D3D12Device::D3D12Device()
	{
	}

	D3D12Device::~D3D12Device()
	{
	}

	bool D3D12Device::Initialize()
	{
		HRESULT hr = S_OK;

		UINT flag = 0;
#if _DEBUG
		//flag |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		hr = CreateDXGIFactory2(flag, IID_PPV_ARGS(&m_factory));

		if (ALPHA_CHECK_FATAL(SUCCEEDED(hr), L"DXGIファクトリーの作成に失敗しました"))
		{
			return false;
		}

		bool performance = true;
		DXGI_GPU_PREFERENCE per = DXGI_GPU_PREFERENCE_UNSPECIFIED;

		ComPtr<IDXGIAdapter4> adapter = nullptr;
		per = performance ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER;

		hr = m_factory->EnumAdapterByGpuPreference(0, per, IID_PPV_ARGS(&m_adapter));

		if (ALPHA_CHECK_FATAL(SUCCEEDED(hr), L"DXGIアダプター作成に失敗しました"))
		{
			return false;
		}

		// デバイス作成
#if _DEBUG
		ComPtr<ID3D12Debug3> debug;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));

		debug->EnableDebugLayer();
		debug->SetEnableGPUBasedValidation(false);

		ComPtr<ID3D12DeviceRemovedExtendedDataSettings> pDredSettings;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDredSettings))))
		{
			pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		}
#endif
		hr = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_device));
		if (ALPHA_CHECK_FATAL(SUCCEEDED(hr), L"D3D12デバイスの作成に失敗しました"))
		{
			return false;
		}

		ComPtr<ID3D12InfoQueue> info;
		if (SUCCEEDED(m_device.As(&info)))
		{
			// 深刻なエラーの時だけデバッガを止める
			info->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			info->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

			D3D12_INFO_QUEUE_FILTER filter = {};
			D3D12_MESSAGE_ID denyIds[] = { D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE };
			filter.DenyList.NumIDs = _countof(denyIds);
			filter.DenyList.pIDList = denyIds;
			info->PushStorageFilter(&filter);
		}

		D3D12_FEATURE_DATA_D3D12_OPTIONS5 options = {};
		m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options, sizeof(options));

		if (options.RaytracingTier < D3D12_RAYTRACING_TIER_1_2)
		{
			ALPHA_LOG_ERROR(L"meshshadernodeが対応していません;;");
		}

		return true;
	}

	void D3D12Device::RequestTopLevelASSize(const RHIBuildTLASDesc& desc, RHIASMemoryInfo* info)
	{
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
		inputs.NumDescs = desc.instance_count;
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO build_info = {};
		m_device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &build_info);

		info->size = build_info.ResultDataMaxSizeInBytes;
		info->scratch_size = build_info.ScratchDataSizeInBytes;
		info->update_size = build_info.UpdateScratchDataSizeInBytes;
	}

	void D3D12Device::RequestBottomLevelASSize(const RHIBuildBLASDesc& desc, RHIASMemoryInfo* info)
	{
		UInt32 geometry_count = desc.geometries.size();
		TArray<D3D12_RAYTRACING_GEOMETRY_DESC> geometry_desc(geometry_count);

		bool is_traiangle = (desc.vertex_count > 0);

		if (is_traiangle)
		{
			for (UInt32 i = 0; i < geometry_count; ++i)
			{
				auto& geometry = geometry_desc[i];
				geometry.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
				geometry.Flags = (desc.geometries[i].is_opaque) ? D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE : D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;

				geometry.Triangles.VertexBuffer.StrideInBytes = sizeof(Vector3);
				geometry.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				geometry.Triangles.VertexCount = desc.vertex_count;


				geometry.Triangles.IndexCount = desc.geometries[i].index_count;
				geometry.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;

			}
		}
		else
		{
			for (UInt32 i = 0; i < geometry_count; ++i)
			{
				auto& geometry = geometry_desc[i];
				geometry.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
				geometry.Flags = (desc.geometries[i].is_opaque) ? D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE : D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;

				geometry.AABBs.AABBCount = desc.geometries[i].aabb_count;
				geometry.AABBs.AABBs.StrideInBytes = sizeof(D3D12_RAYTRACING_AABB);
			}

		}

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;

		if (desc.allow_update) {
			flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
		}
		else {
			flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
		}

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.Flags = flags;
		inputs.NumDescs = geometry_count;
		inputs.pGeometryDescs = geometry_desc.data();
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO build_info = {};
		m_device->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &build_info);

		info->size = build_info.ResultDataMaxSizeInBytes;
		info->scratch_size = build_info.ScratchDataSizeInBytes;
		info->update_size = build_info.UpdateScratchDataSizeInBytes;

	}

}
#endif