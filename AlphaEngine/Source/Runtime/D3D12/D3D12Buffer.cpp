#include "D3D12Buffer.h"

#include "RHI/Definitions.h"

#if RHI_D3D12

namespace
{
	using namespace Alpha;

	static D3D12_HEAP_TYPE GetD3D12HeapType(ERHIResourceAccessType access)
	{
		switch (access) 
		{
		case ERHIResourceAccessType::Static	: return D3D12_HEAP_TYPE_DEFAULT;
		case ERHIResourceAccessType::Dynamic: return D3D12_HEAP_TYPE_UPLOAD;
		case ERHIResourceAccessType::Staging: return D3D12_HEAP_TYPE_READBACK;
		default: return D3D12_HEAP_TYPE_DEFAULT;
		}
	}

	static D3D12_RESOURCE_FLAGS GetD3D12ResourceFlags(ERHIResourceBindFlags bind)
	{
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
		if (EnumHasFlag(bind, ERHIResourceBindFlags::UnorderedAccess))
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		if(EnumHasFlag(bind,ERHIResourceBindFlags::RayTracingAS))
			flags |= D3D12_RESOURCE_FLAG_RAYTRACING_ACCELERATION_STRUCTURE;

		return flags;
	}

	static D3D12_RESOURCE_STATES GetD3D12ResourceStates(ERHIResourceAccessType access)
	{
		switch (access) 
		{
		case ERHIResourceAccessType::Static	: return D3D12_RESOURCE_STATE_COMMON;
		case ERHIResourceAccessType::Dynamic: return D3D12_RESOURCE_STATE_GENERIC_READ;
		case ERHIResourceAccessType::Staging: return D3D12_RESOURCE_STATE_COPY_DEST;
		default:						return D3D12_RESOURCE_STATE_COMMON;
		}
	}

	static ERHIResourceState GetResourceStates(D3D12_RESOURCE_STATES d3d12_states)
	{
		if (d3d12_states == D3D12_RESOURCE_STATE_COMMON)
		{
			return ERHIResourceState::Common;
		}
		if (d3d12_states == D3D12_RESOURCE_STATE_PRESENT)
		{
			return ERHIResourceState::Present;
		}
		if (d3d12_states == D3D12_RESOURCE_STATE_GENERIC_READ)
		{
			return ERHIResourceState::GenericRead;
		}

		ERHIResourceState result = ERHIResourceState::Common;

		if (d3d12_states & D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
		{
			result |= ERHIResourceState::VertexConstantBuffer;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_INDEX_BUFFER)
		{
			result |= ERHIResourceState::IndexBuffer;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_RENDER_TARGET)
		{
			result |= ERHIResourceState::RenderTarget;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		{
			result |= ERHIResourceState::UnorderedAccess;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_DEPTH_WRITE)
		{
			result |= ERHIResourceState::DepthWrite;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_DEPTH_READ)
		{
			result |= ERHIResourceState::DepthRead;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
		{
			result |= ERHIResourceState::NonPixelShaderResource;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
		{
			result |= ERHIResourceState::PixelShaderResource;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_STREAM_OUT)
		{
			result |= ERHIResourceState::StreamOut;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT)
		{
			result |= ERHIResourceState::IndirectArgument;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_COPY_DEST)
		{
			result |= ERHIResourceState::CopyDestination;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_COPY_SOURCE)
		{
			result |= ERHIResourceState::CopySource;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_RESOLVE_DEST)
		{
			result |= ERHIResourceState::ResolveDestination;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_RESOLVE_SOURCE)
		{
			result |= ERHIResourceState::ResolveSource;
		}
		if (d3d12_states & D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE)
		{
			result |= ERHIResourceState::RaytracingStructure;
		}

		return result;

	}

	static ERHIResourceState GetRHIResourceState(ERHIResourceAccessType access)
	{
		if (access == ERHIResourceAccessType::Dynamic)
		{
			return ERHIResourceState::GenericRead;
		}
		return ERHIResourceState::Common;
	}
}

namespace Alpha
{
	D3D12Buffer::D3D12Buffer(D3D12Device* device)
		: RHIBuffer()
		, D3D12DeviceChild(device)
	{
	}
	
	D3D12Buffer::~D3D12Buffer()
	{
	}

	bool D3D12Buffer::Initialize(const RHIBufferDesc& desc)
	{
		auto device = GetD3D12Device();

		// 

		D3D12_HEAP_PROPERTIES properties = {};
		properties.Type = GetD3D12HeapType(desc.access);
		properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		properties.CreationNodeMask = 1;
		properties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resource_desc = {};
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment = 0;
		resource_desc.Width = desc.size;
		resource_desc.Height = 1;
		resource_desc.DepthOrArraySize = 1;
		resource_desc.MipLevels = 1;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 1;
		resource_desc.SampleDesc.Quality = 0;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags = GetD3D12ResourceFlags(desc.flags);

		auto states = GetD3D12ResourceStates(desc.access);

		if (EnumHasFlag(desc.flags, ERHIResourceBindFlags::UnorderedAccess))
		{
			states = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		}
		if (EnumHasFlag(desc.flags, ERHIResourceBindFlags::RayTracingAS))
		{
			states = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		}
		auto hr = device->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &resource_desc, states, nullptr, IID_PPV_ARGS(&m_resource));
		if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"リソースの作成に失敗しました"))
		{
			return false;
		}
		SetSize(desc.size);
		SetStride(desc.stride);
		SetAccessType(desc.access);
		SetFlags(desc.flags);

		SetState(GetResourceStates(states));

		return true;
	}

	void D3D12Buffer::Write(void* transfer_data)
	{
		uint8_t* map_data = nullptr;
		// 0,0で全部になる
		D3D12_RANGE range = {};
		range.Begin = 0;
		range.End = 0;
		auto size = GetSize();
		auto hr = m_resource->Map(0, &range, (void**)&map_data);
		if (FAILED(hr))
		{
			ALPHA_LOG_ERROR(L"バッファのマップに失敗しました");
		}
		memcpy(map_data, transfer_data, size);

		range.End = size;
		m_resource->Unmap(0, &range);
	}

	UInt8* D3D12Buffer::Map(UInt64 read_begin, UInt64 read_end)
	{
		if (GetAccessType() != ERHIResourceAccessType::Dynamic)
			return nullptr;

		uint8_t* map_data = nullptr;

		D3D12_RANGE range = {};
		range.Begin = read_begin;
		range.End = read_end;

		auto hr = m_resource->Map(0, &range, (void**)&map_data);
		if (FAILED(hr))
		{
			ALPHA_LOG_ERROR(L"バッファのマップに失敗しました");
			// Map失敗
			return nullptr;
		}

		return map_data;
	}

	void D3D12Buffer::Unmap(UInt64 written_begin, UInt64 written_end)
	{
		D3D12_RANGE range = {};
		range.Begin = written_begin;
		range.End = written_end;
		m_resource->Unmap(0, &range);
	}

	UInt64 D3D12Buffer::GetGPUAddress() const
	{
		return m_resource->GetGPUVirtualAddress();
	}

	void D3D12Buffer::SetName(StringView name)
	{
		m_resource->SetName(name.c_str());
	}
}

#endif