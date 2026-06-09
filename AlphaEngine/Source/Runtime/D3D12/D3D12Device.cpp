#include "D3D12Device.h"

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

		if (ALPHA_CHECK_FATAL(FAILED(hr), L"DXGIファクトリーの作成に失敗しました"))
		{
			return false;
		}

		bool performance = true;
		DXGI_GPU_PREFERENCE per = DXGI_GPU_PREFERENCE_UNSPECIFIED;

		ComPtr<IDXGIAdapter4> adapter = nullptr;
		per = performance ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_MINIMUM_POWER;

		hr = m_factory->EnumAdapterByGpuPreference(0, per, IID_PPV_ARGS(&m_adapter));

		if (ALPHA_CHECK_FATAL(FAILED(hr), L"DXGIアダプター作成に失敗しました"))
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
		if (ALPHA_CHECK_FATAL(FAILED(hr), L"D3D12デバイスの作成に失敗しました"))
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

		return true;
	}

}
#endif