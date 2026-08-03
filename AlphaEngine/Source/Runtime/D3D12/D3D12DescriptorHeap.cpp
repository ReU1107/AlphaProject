#include "D3D12DescriptorHeap.h"
#include "RHI/Definitions.h"
#include "RHI/RHIResource.h"
#include "RHI/RHIBuffer.h"

#if RHI_D3D12

namespace
{
	using namespace Alpha;
	static D3D12_DESCRIPTOR_HEAP_TYPE GetD3D12DescriptorHeapType(ERHIDescriptorType type)
	{
		switch (type)
		{
		case ERHIDescriptorType::SRV_UAV_CBV	: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		case ERHIDescriptorType::RTV			: return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		case ERHIDescriptorType::DSV			: return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		case ERHIDescriptorType::Sampler		: return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		default:
			break;
		}
		return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	}


	static bool CheckType(ERHIResourceViewType view_type, ERHIDescriptorType heap_type)
	{
		switch (view_type)
		{
		case ERHIResourceViewType::ShaderResourceView	: return (heap_type == ERHIDescriptorType::SRV_UAV_CBV);
		case ERHIResourceViewType::UnorderedAccessView	: return (heap_type == ERHIDescriptorType::SRV_UAV_CBV);
		case ERHIResourceViewType::ConstantBufferView	: return (heap_type == ERHIDescriptorType::SRV_UAV_CBV);
		case ERHIResourceViewType::RenderTargetView		: return (heap_type == ERHIDescriptorType::RTV);
		case ERHIResourceViewType::DepthStencilView		: return (heap_type == ERHIDescriptorType::DSV);
		default:
			break;
		}

		return false;
	}

	static String GetViewTypeName(ERHIResourceViewType view_type)
	{
		switch (view_type)
		{
		case ERHIResourceViewType::ShaderResourceView	: return L"SRV";
		case ERHIResourceViewType::UnorderedAccessView	: return L"UAV";
		case ERHIResourceViewType::ConstantBufferView	: return L"CBV";
		case ERHIResourceViewType::RenderTargetView		: return L"RTV";
		case ERHIResourceViewType::DepthStencilView		: return L"DSV";
		default:
			break;
		}
		return L"Unknown";
	}

	static String GetHeapTypeName(ERHIDescriptorType heap_type)
	{
		switch (heap_type)
		{
		case ERHIDescriptorType::SRV_UAV_CBV	: return L"SRV_UAV_CBV";
		case ERHIDescriptorType::RTV			: return L"RTV";
		case ERHIDescriptorType::DSV			: return L"DSV";
		case ERHIDescriptorType::Sampler		: return L"Sampler";
		default:
			break;
		}
		return L"Unknown";
	}

	static void CreateRenderTargetView(ID3D12Device7* device, ID3D12Resource1* resource, D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		auto resource_desc = resource->GetDesc();
		D3D12_RENDER_TARGET_VIEW_DESC view_desc = {};

		switch (resource_desc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (resource_desc.SampleDesc.Count != 1)
			{
				if (resource_desc.DepthOrArraySize > 1)
				{
					view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
					view_desc.Texture2DMSArray.ArraySize = resource_desc.DepthOrArraySize;
					view_desc.Texture2DMSArray.FirstArraySlice = 0;
				}
				else
				{
					view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
					view_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
				}
			}
			else
			{
				if (resource_desc.DepthOrArraySize > 1)
				{
					view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
					view_desc.Texture2DArray.ArraySize = resource_desc.DepthOrArraySize;
					view_desc.Texture2DArray.FirstArraySlice = 0;
					view_desc.Texture2DArray.MipSlice = 0;
					view_desc.Texture2DArray.PlaneSlice = 0;
				}
				else
				{
					view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
					view_desc.Texture2D.MipSlice = 0;
					view_desc.Texture2D.PlaneSlice = 0;
				}
			}
			break;
		default:
			break;
		}

		device->CreateRenderTargetView(resource, &view_desc, handle);

	}

	static void CreateDepthStencilView(ID3D12Device7* device, ID3D12Resource1* resource, D3D12_CPU_DESCRIPTOR_HANDLE handle)
	{
		auto resource_desc = resource->GetDesc();
		D3D12_DEPTH_STENCIL_VIEW_DESC view_desc = {};
		view_desc.Format = resource_desc.Format;
		view_desc.Flags = D3D12_DSV_FLAG_NONE;

		switch (resource_desc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (resource_desc.SampleDesc.Count != 1)
			{
				// マルチサンプリングあり
				if (resource_desc.DepthOrArraySize > 1)
				{
					view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
					view_desc.Texture2DMSArray.ArraySize = resource_desc.DepthOrArraySize;
					view_desc.Texture2DMSArray.FirstArraySlice = 0;
				}
				else
				{
					view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
					view_desc.Texture2DMS.UnusedField_NothingToDefine = 0;
				}
			}
			else
			{
				// マルチサンプリングなし
				if (resource_desc.DepthOrArraySize > 1)
				{
					view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
					view_desc.Texture2DArray.ArraySize = resource_desc.DepthOrArraySize;
					view_desc.Texture2DArray.FirstArraySlice = 0;
					view_desc.Texture2DArray.MipSlice = 0;
				}
				else
				{
					view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
					view_desc.Texture2D.MipSlice = 0;
				}
			}
			break;
		default:
			break;
		}

		device->CreateDepthStencilView(resource, &view_desc, handle);
	}

	static void CreateShaderResourceView(ID3D12Device7* device, ID3D12Resource1* resource, D3D12_CPU_DESCRIPTOR_HANDLE handle, UINT resource_stride, ERHIResourceBindFlags flags)
	{
		auto resource_desc = resource->GetDesc();
		auto stride = resource_stride; // バッファ用

		D3D12_SHADER_RESOURCE_VIEW_DESC view_desc = {};
		view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if (EnumHasFlag(flags, ERHIResourceBindFlags::RayTracingAS))
		{
			// レイトレーシング用
			view_desc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
			view_desc.RaytracingAccelerationStructure.Location = resource->GetGPUVirtualAddress();
			resource = nullptr;
		}
		else
		{
			// 深度値用をシェーダーリソースとして使用する場合
			switch (resource_desc.Format)
			{
			case DXGI_FORMAT_D32_FLOAT:
				view_desc.Format = DXGI_FORMAT_R32_FLOAT;
				break;
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
				view_desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
				break;
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			case DXGI_FORMAT_D16_UNORM:
				view_desc.Format = DXGI_FORMAT_R16_UNORM;
				break;
			default:
				view_desc.Format = resource_desc.Format;
				break;
			}

			switch (resource_desc.Dimension)
			{
			case D3D12_RESOURCE_DIMENSION_UNKNOWN:
				// ここに入ってきたらTextureCubeの可能性があるのかな???
				// わからんから今はとめておく
				//assert(!"リソースのDimensionがおかしいです");
				break;
			case D3D12_RESOURCE_DIMENSION_BUFFER:
				view_desc.Format = DXGI_FORMAT_UNKNOWN;
				view_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				view_desc.Buffer.FirstElement = 0;
				view_desc.Buffer.NumElements = (UINT)resource_desc.Width / stride;
				// ByteAddressBufferを使用する場合はこちら
				if (EnumHasFlag(flags, ERHIResourceBindFlags::Raw))
				{
					view_desc.Buffer.StructureByteStride = 0;
					view_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
					view_desc.Format = DXGI_FORMAT_R32_TYPELESS;
				}
				else
				{
					view_desc.Buffer.StructureByteStride = stride;
					view_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
				}
				break;
			case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
				if (resource_desc.DepthOrArraySize > 1)
				{
					view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
					view_desc.Texture1DArray.ArraySize = resource_desc.DepthOrArraySize;
					view_desc.Texture1DArray.FirstArraySlice = 0;
					view_desc.Texture1DArray.MipLevels = 0;
					view_desc.Texture1DArray.MostDetailedMip = 0;
					view_desc.Texture1DArray.ResourceMinLODClamp = 0;
				}
				else
				{
					view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
					view_desc.Texture1D.MipLevels = 0;
					view_desc.Texture1D.MostDetailedMip = 0;
					view_desc.Texture1D.ResourceMinLODClamp = 0.0f;
				}

				break;
			case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
				// マルチサンプリング
				if (resource_desc.SampleDesc.Count != 1)
				{
					if (resource_desc.DepthOrArraySize > 1)
					{
						view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
						view_desc.Texture2DMSArray.ArraySize = resource_desc.DepthOrArraySize;
						view_desc.Texture2DMSArray.FirstArraySlice = 0;
					}
					else
					{
						view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
						//nativeDesc.Texture2DMS.UnusedField_NothingToDefine;

					}
				}
				else
				{
					if (resource_desc.DepthOrArraySize > 1)
					{
						view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
						view_desc.Texture2DArray.ArraySize = resource_desc.DepthOrArraySize;
						view_desc.Texture2DArray.FirstArraySlice = 0;
						view_desc.Texture2DArray.MipLevels = resource_desc.MipLevels;
						view_desc.Texture2DArray.MostDetailedMip = resource_desc.MipLevels;
						view_desc.Texture2DArray.ResourceMinLODClamp = 0;
					}
					else
					{
						view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
						view_desc.Texture2D.MipLevels = resource_desc.MipLevels;
						view_desc.Texture2D.MostDetailedMip = 0;
						view_desc.Texture2D.ResourceMinLODClamp = 0;

					}

				}
				break;
			case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			{
				view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
				view_desc.Texture3D.MipLevels = resource_desc.MipLevels;
				view_desc.Texture3D.MostDetailedMip = resource_desc.MipLevels - 1;
				view_desc.Texture3D.ResourceMinLODClamp = 0;

			}
			break;
			default:
				return;
			}

		}
		device->CreateShaderResourceView(resource, &view_desc, handle);
	}


	static void CreateUnorderedAccessView(ID3D12Device7* device, ID3D12Resource1* resource, ID3D12Resource1* counter_resource, D3D12_CPU_DESCRIPTOR_HANDLE handle, UINT resource_stride, ERHIResourceBindFlags flags)
	{
		auto resource_desc = resource->GetDesc();
		auto stride = resource_stride; // バッファ用
		D3D12_UNORDERED_ACCESS_VIEW_DESC view_desc = {};
		view_desc.Format = resource_desc.Format;
		switch (resource_desc.Dimension)
		{
		case D3D12_RESOURCE_DIMENSION_BUFFER:
			view_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			view_desc.Buffer.FirstElement = 0;
			view_desc.Buffer.CounterOffsetInBytes = 0;
			view_desc.Buffer.NumElements = (UINT)resource_desc.Width / stride;

			// ByteAddressBufferを使用する場合はこちら
			if (EnumHasFlag(flags, ERHIResourceBindFlags::Raw))
			{
				view_desc.Buffer.StructureByteStride = 0;
				view_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
				view_desc.Format = DXGI_FORMAT_R32_TYPELESS;
			}
			else
			{
				view_desc.Buffer.StructureByteStride = stride;
				view_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
			if (resource_desc.DepthOrArraySize > 1)
			{
				view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
				view_desc.Texture1DArray.ArraySize = 0;
				view_desc.Texture1DArray.FirstArraySlice = 0;
				view_desc.Texture1DArray.MipSlice = 0;
			}
			else
			{
				view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
				view_desc.Texture1D.MipSlice = 0;
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
			if (resource_desc.DepthOrArraySize > 1)
			{
				view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				view_desc.Texture2DArray.ArraySize = 0;
				view_desc.Texture2DArray.FirstArraySlice = 0;
				view_desc.Texture2DArray.MipSlice = 0;
			}
			else
			{
				view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				view_desc.Texture2D.MipSlice = 0;
				view_desc.Texture2D.PlaneSlice = 0;
			}
			break;
		case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
			view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			view_desc.Texture3D.MipSlice = 0;
			view_desc.Texture3D.FirstWSlice = 0;
			view_desc.Texture3D.WSize = -1;
			break;
		case D3D12_RESOURCE_DIMENSION_UNKNOWN:
		default:
			return;
		}
		device->CreateUnorderedAccessView(resource, counter_resource, &view_desc, handle);

	}

	static void CreateConstantBufferView(ID3D12Device7* device, ID3D12Resource1* resource, D3D12_CPU_DESCRIPTOR_HANDLE handle, UINT64 offset, UINT size)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC view_desc = {};

		view_desc.BufferLocation = resource->GetGPUVirtualAddress() + offset;
		view_desc.SizeInBytes = size;

		device->CreateConstantBufferView(&view_desc, handle);
	}
}

namespace Alpha
{
	D3D12DescriptorHeap::D3D12DescriptorHeap(D3D12Device* device)
		: RHIDescriptorHeap()
		, D3D12DeviceChild(device)
	{
	}

	D3D12DescriptorHeap::~D3D12DescriptorHeap()
	{
	}

	bool D3D12DescriptorHeap::Initialize(const RHIDescriptorHeapDesc& desc)
	{
		D3D12_DESCRIPTOR_HEAP_TYPE type = GetD3D12DescriptorHeapType(desc.type);

		D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
		heap_desc.NumDescriptors = (UINT)desc.max_count;
		heap_desc.Type = type;

		if (desc.type == ERHIDescriptorType::RTV || desc.type == ERHIDescriptorType::DSV)
			heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		else
			heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heap_desc.NodeMask = 0;

		auto device = GetD3D12Device();

		auto hr = device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&m_heap));
		if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"デスクリプターヒープの作成に失敗しました"))
		{
			return false;
		}

		SetType(desc.type);

		return true;
	}

	bool D3D12DescriptorHeap::CreateView(const RHIResourceViewDesc& desc)
	{
		auto device = GetD3D12Device();

		auto view_type = desc.type;
		auto heap_type = GetType();
		auto view_name = GetViewTypeName(view_type);
		auto heap_name = GetHeapTypeName(heap_type);

		//if (ALPHA_CHECK_ERROR(!CheckType(view_type, heap_type), std::format(L"ビューとヒープのタイプ不一致 ViewType:{} HeapType:{}", view_name, heap_name).c_str()))
		//{
		//	return false;
		//}

		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = GetCPUHandle(desc.heap_index);
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = GetGPUHandle(desc.heap_index);

		auto buffer = desc.resource->As<RHIBuffer>();
		UINT resource_stride = 4;
		if (buffer)
		{
			resource_stride = buffer->GetStride();
		}

		auto resource = static_cast<ID3D12Resource1*>(desc.resource->GetNative());
		ID3D12Resource1* counter_resource = nullptr;

		switch (desc.type)
		{
		case ERHIResourceViewType::Unknown:
			break;
		case ERHIResourceViewType::ShaderResourceView:
			CreateShaderResourceView(device, resource, cpu_handle, resource_stride, desc.resource->GetFlags());
			break;
		case ERHIResourceViewType::RenderTargetView:
			CreateRenderTargetView(device, resource, cpu_handle);
			break;
		case ERHIResourceViewType::DepthStencilView:
			CreateDepthStencilView(device, resource, cpu_handle);
			break;
		case ERHIResourceViewType::UnorderedAccessView:
			CreateUnorderedAccessView(device, resource, counter_resource, cpu_handle, resource_stride, desc.resource->GetFlags());
			break;
		case ERHIResourceViewType::ConstantBufferView:
			//CreateConstantBufferView(device, resource, cpu_handle, offset, size);
			break;
		default:
			break;
		}

		return true;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetCPUHandle(UInt32 index) const
	{
		auto device = GetD3D12Device();

		UINT descriptor_size = device->GetDescriptorHandleIncrementSize(GetD3D12DescriptorHeapType(GetType()));

		uint64_t offset = (uint64_t)descriptor_size * index;

		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = m_heap->GetCPUDescriptorHandleForHeapStart();
		cpu_handle.ptr += offset;

		return cpu_handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetGPUHandle(UInt32 index) const
	{
		auto device = GetD3D12Device();

		auto desc = m_heap->GetDesc();

		if (!(desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE))
		{
			return { 0 };
		}

		UINT descriptor_size = device->GetDescriptorHandleIncrementSize(GetD3D12DescriptorHeapType(GetType()));

		uint64_t offset = (uint64_t)descriptor_size * index;

		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = m_heap->GetGPUDescriptorHandleForHeapStart();
		gpu_handle.ptr += offset;

		return gpu_handle;
	}

}

#endif