#include "D3D12CommandBuffer.h"
#include "RHI/Definitions.h"
#include "RHI/Constants.h"

#include "D3D12Buffer.h"
#include "D3D12Texture.h"
#include "D3D12GraphicsPipeline.h"
#include "D3D12ComputePipeline.h"
#include "D3D12RayTracePipeline.h"
#include "D3D12PipelineLayout.h"
#include "D3D12CommandLayout.h"
#include "D3D12DescriptorHeap.h"

#include "Core/Assertion.h"

#if RHI_D3D12

/*
* CopyTypeで作成した場合
* Transtionでの遷移は
		 → CopySource,
* Common
		 ← CopyDestination
* が可能、他のステートには遷移できない
* そのためコピー先のバッファをCommonにした後に描画を担当するコマンドバッファでVertexConstantBuffer等に遷移させる必要がある
*/


namespace
{
	using namespace Alpha;
	// 最大遷移数

	static D3D12_RESOURCE_STATES GetD3D12ResourceStates(Alpha::ERHIResourceState state)
	{
		switch (state)
		{
		case Alpha::ERHIResourceState::Common					: return D3D12_RESOURCE_STATE_COMMON;
		case Alpha::ERHIResourceState::VertexConstantBuffer		: return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		case Alpha::ERHIResourceState::IndexBuffer				: return D3D12_RESOURCE_STATE_INDEX_BUFFER;
		case Alpha::ERHIResourceState::RenderTarget				: return D3D12_RESOURCE_STATE_RENDER_TARGET;
		case Alpha::ERHIResourceState::UnorderedAccess			: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		case Alpha::ERHIResourceState::DepthWrite				: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
		case Alpha::ERHIResourceState::DepthRead				: return D3D12_RESOURCE_STATE_DEPTH_READ;
		case Alpha::ERHIResourceState::NonPixelShaderResource	: return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		case Alpha::ERHIResourceState::PixelShaderResource		: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		case Alpha::ERHIResourceState::IndirectArgument			: return D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		case Alpha::ERHIResourceState::CopyDestination			: return D3D12_RESOURCE_STATE_COPY_DEST;
		case Alpha::ERHIResourceState::CopySource				: return D3D12_RESOURCE_STATE_COPY_SOURCE;
		case Alpha::ERHIResourceState::ResolveDestination		: return D3D12_RESOURCE_STATE_RESOLVE_DEST;
		case Alpha::ERHIResourceState::ResolveSource			: return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		case Alpha::ERHIResourceState::RaytracingStructure		: return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		case Alpha::ERHIResourceState::GenericRead				: return D3D12_RESOURCE_STATE_GENERIC_READ;
		case Alpha::ERHIResourceState::Present					: return D3D12_RESOURCE_STATE_PRESENT;
		case Alpha::ERHIResourceState::AllShaderResource		: return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		default:
			return D3D12_RESOURCE_STATE_COMMON;
		}
	}

	static D3D12_COMMAND_LIST_TYPE GetD3D12CommandType(Alpha::ERHICommandType type)
	{
		switch (type)
		{
		case Alpha::ERHICommandType::Direct:	return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case Alpha::ERHICommandType::Bundle:	return D3D12_COMMAND_LIST_TYPE_BUNDLE;
		case Alpha::ERHICommandType::Compute:	return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case Alpha::ERHICommandType::Copy:		return D3D12_COMMAND_LIST_TYPE_COPY;
		default:
			break;
		}
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	}

	static D3D12_PRIMITIVE_TOPOLOGY GetD3D12PrimitiveTopology(Alpha::ERHIPrimitiveTopology topology)
	{
		switch (topology)
		{
		case Alpha::ERHIPrimitiveTopology::PointList		: return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case Alpha::ERHIPrimitiveTopology::LineList			: return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case Alpha::ERHIPrimitiveTopology::LineStrip		: return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case Alpha::ERHIPrimitiveTopology::TriangleList		: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case Alpha::ERHIPrimitiveTopology::TriangleStrip	: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		default:
			break;
		}
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

}

namespace Alpha
{
	D3D12CommandBuffer::D3D12CommandBuffer(D3D12Device* device)
		: RHICommandBuffer()
		, D3D12DeviceChild(device) 
	{
	}

	D3D12CommandBuffer::~D3D12CommandBuffer()
	{
		//m_list->ExecuteIndirect()
	}

	bool D3D12CommandBuffer::Initialize(const RHICommandBufferDesc& desc)
	{
		auto device = GetD3D12Device();
				
		auto count = desc.count;
		D3D12_COMMAND_LIST_TYPE type = GetD3D12CommandType(desc.type);
		m_allocators.resize(count);
		 
		HRESULT hr = S_OK;
		for (UInt32 i = 0; i < count; i++)
		{
			hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_allocators[i]));

			if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"コマンドアロケーターの作成に失敗しました"))
			{
				return false;
			}
		}

		hr = device->CreateCommandList1(0, type, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_list));

		if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"コマンドリストの作成に失敗しました"))
		{
			return false;
		}
		return true;
	}

	void D3D12CommandBuffer::SetName(StringView name)
	{
		m_list->SetName(name.c_str());
	}

	void D3D12CommandBuffer::CopyTexture(RHITexture* src, RHITexture* dst)
	{
	}

	void D3D12CommandBuffer::CopyTextureRegion(RHIBuffer* src, UInt64 src_offset, RHITexture* dst)
	{
		auto* d3d_dst = static_cast<D3D12Texture*>(dst);
		auto* d3d_src = static_cast<D3D12Buffer*>(src);

		// 一旦サブリソース0だけで
		// TODO: みっぷマップとか必要になってきたらまた別途作る
		const UInt32 subresource_index = 0;
		const auto& layout = d3d_dst->GetLayout(subresource_index);

		D3D12_TEXTURE_COPY_LOCATION dst_location = {};
		dst_location.pResource = (ID3D12Resource*)d3d_dst->GetNative();
		dst_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst_location.SubresourceIndex = subresource_index;

		D3D12_TEXTURE_COPY_LOCATION src_location = {};
		src_location.pResource = (ID3D12Resource*)d3d_src->GetNative();
		src_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src_location.PlacedFootprint = layout.footprint;
		src_location.PlacedFootprint.Offset += src_offset;

		m_list->CopyTextureRegion(&dst_location, 0, 0, 0, &src_location, nullptr);
	}

	void D3D12CommandBuffer::CopyBuffer(RHIBuffer* src, RHIBuffer* dst)
	{
		auto src_native = static_cast<ID3D12Resource1*>(src->GetNative());
		auto dst_native = static_cast<ID3D12Resource1*>(dst->GetNative());

		m_list->CopyResource(dst_native, src_native);
	}

	void D3D12CommandBuffer::CopyBufferRegion(RHIBuffer* src, UInt64 src_offset, RHIBuffer* dst, UInt64 dst_offset, UInt64 size)
	{
		auto src_native = static_cast<ID3D12Resource1*>(src->GetNative());
		auto dst_native = static_cast<ID3D12Resource1*>(dst->GetNative());

		m_list->CopyBufferRegion(dst_native, dst_offset, src_native, src_offset, size);
	}

	void D3D12CommandBuffer::Begin(UInt32 index)
	{
		HRESULT hr = S_OK;
#if _DEBUG
		hr = m_allocators[index]->Reset();
		if (FAILED(hr))
		{

		}

		hr = m_list->Reset(m_allocators[index], nullptr);
		if (FAILED(hr))
		{
		}
#else
		// 毎フレーム呼び出すものなのでdebug以外でifチェックを行わない
		m_allocators[index]->Reset();
		m_list->Reset(m_allocators[index], nullptr);
#endif

	}

	void D3D12CommandBuffer::End()
	{
		HRESULT hr = m_list->Close();

		if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"積み込んだコマンドに問題があります"))
		{

		}
	}

	void D3D12CommandBuffer::SetGraphicsPipelineLayout(RHIPipelineLayout* layout)
	{
		auto d3d12_layout = static_cast<D3D12PipelineLayout*>(layout);
		m_list->SetGraphicsRootSignature(d3d12_layout->GetNativeHandle());
	}

	void D3D12CommandBuffer::SetComputePipelineLayout(RHIPipelineLayout* layout)
	{
		auto d3d12_layout = static_cast<D3D12PipelineLayout*>(layout);
		m_list->SetComputeRootSignature(d3d12_layout->GetNativeHandle());
	}

	void D3D12CommandBuffer::SetGraphicsPipeline(RHIGraphicsPipeline* pipeline, RHIPipelineLayout* layout)
	{
		auto d3d12_layout = static_cast<D3D12PipelineLayout*>(layout);
		auto d3d12_pipeline = static_cast<D3D12GraphicsPipeline*>(pipeline);

		m_list->SetPipelineState(d3d12_pipeline->GetNativeHandle());
		m_list->SetGraphicsRootSignature(d3d12_layout->GetNativeHandle());
	}

	void D3D12CommandBuffer::SetComputePipeline(RHIComputePipeline* pipeline, RHIPipelineLayout* layout)
	{
		auto d3d12_layout = static_cast<D3D12PipelineLayout*>(layout);
		auto d3d12_pipeline = static_cast<D3D12ComputePipeline*>(pipeline);

		m_list->SetPipelineState(d3d12_pipeline->GetNativeHandle());
		m_list->SetComputeRootSignature(d3d12_layout->GetNativeHandle());
	}

	void D3D12CommandBuffer::SetRayTracePipeline(RHIRayTracePipeline* pipeline, RHIPipelineLayout* layout)
	{
		auto d3d12_layout = static_cast<D3D12PipelineLayout*>(layout);
		auto d3d12_pipeline = static_cast<D3D12RayTracePipeline*>(pipeline);

		m_list->SetPipelineState1(d3d12_pipeline->GetNativeHandle());
		m_list->SetComputeRootSignature(d3d12_layout->GetNativeHandle());
	}

	void D3D12CommandBuffer::SetDescriptorHeap(RHIDescriptorHeap* heap)
	{
		auto d3d12_heap = static_cast<D3D12DescriptorHeap*>(heap);
		ID3D12DescriptorHeap* heaps[] = { d3d12_heap->GetNativeHandle() };
		m_list->SetDescriptorHeaps(1, heaps);
	}

	void D3D12CommandBuffer::SetDescriptorHeap(RHIDescriptorHeap* srv_heap, RHIDescriptorHeap* sampler_heap)
	{
		ID3D12DescriptorHeap* heaps[2] = {};
		UInt32 num = 0;

		if (srv_heap)
			heaps[num++] = static_cast<D3D12DescriptorHeap*>(srv_heap)->GetNativeHandle();
		if (sampler_heap)
			heaps[num++] = static_cast<D3D12DescriptorHeap*>(sampler_heap)->GetNativeHandle();

		if (num > 0)
			m_list->SetDescriptorHeaps(num, heaps);
	}

	void D3D12CommandBuffer::SetVertexBuffer(RHIBuffer* vb, UInt32 slot)
	{
		D3D12_VERTEX_BUFFER_VIEW view = {};
		view.BufferLocation = vb->GetGPUAddress();
		view.SizeInBytes = (UINT)vb->GetSize();
		view.StrideInBytes = (UINT)vb->GetStride();

		m_list->IASetVertexBuffers(slot, 1, &view);
	}

	void D3D12CommandBuffer::SetPrimitiveTopology(ERHIPrimitiveTopology topology)
	{
		m_list->IASetPrimitiveTopology(GetD3D12PrimitiveTopology(topology));
	}

	void D3D12CommandBuffer::SetIndexBuffer(RHIBuffer* ib, ERHIIndexFormat format)
	{
		D3D12_INDEX_BUFFER_VIEW ibv = {};
		ibv.BufferLocation = ib->GetGPUAddress();
		ibv.Format = format == ERHIIndexFormat::UInt32 ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		ibv.SizeInBytes = (UINT)ib->GetSize();

		m_list->IASetIndexBuffer(&ibv);
	}

	void D3D12CommandBuffer::SetUniformBuffer(RHIBuffer* cb, UInt32 slot, ERHIPipelineType type)
	{
		D3D12_GPU_VIRTUAL_ADDRESS address = cb->GetGPUAddress();
		// パイプラインタイプで分岐
		switch (type)
		{
		case Alpha::ERHIPipelineType::Graphics:
			m_list->SetGraphicsRootConstantBufferView(slot, address);
			break;
		case Alpha::ERHIPipelineType::Compute:
			m_list->SetComputeRootConstantBufferView(slot, address);
			break;
		case Alpha::ERHIPipelineType::RayTracing:
			m_list->SetComputeRootConstantBufferView(slot, address);
			break;
		default:
			break;
		}

	}

	void D3D12CommandBuffer::SetGraphicsTable(RHIDescriptorHeap* heap, UInt32 index, UInt32 slot)
	{
		auto d3d12_heap = static_cast<D3D12DescriptorHeap*>(heap);
		m_list->SetGraphicsRootDescriptorTable(slot, d3d12_heap->GetGPUHandle(index));
	}

	void D3D12CommandBuffer::SetComputeTable(RHIDescriptorHeap* heap, UInt32 index, UInt32 slot)
	{
		auto d3d12_heap = static_cast<D3D12DescriptorHeap*>(heap);
		m_list->SetComputeRootDescriptorTable(slot, d3d12_heap->GetGPUHandle(index));
	}

	void D3D12CommandBuffer::SetGraphicsUAV(RHIBuffer* buffer, UInt32 slot)
	{
		D3D12_GPU_VIRTUAL_ADDRESS address = buffer->GetGPUAddress();

		m_list->SetGraphicsRootUnorderedAccessView(slot, address);
	}

	void D3D12CommandBuffer::SetComputeUAV(RHIBuffer* buffer, UInt32 slot)
	{
		D3D12_GPU_VIRTUAL_ADDRESS address = buffer->GetGPUAddress();

		m_list->SetComputeRootUnorderedAccessView(slot, address);
	}

	void D3D12CommandBuffer::SetGraphicsSRV(RHIBuffer* buffer, UInt32 slot, UInt64 offset)
	{
		D3D12_GPU_VIRTUAL_ADDRESS address = buffer->GetGPUAddress();
		address += offset;

		m_list->SetGraphicsRootShaderResourceView(slot, address);
	}

	void D3D12CommandBuffer::SetComputeSRV(RHIBuffer* buffer, UInt32 slot, UInt64 offset)
	{
		D3D12_GPU_VIRTUAL_ADDRESS address = buffer->GetGPUAddress();
		address += offset;

		m_list->SetComputeRootShaderResourceView(slot, address);
	}

	void D3D12CommandBuffer::SetGraphicsCBV(RHIBuffer* buffer, UInt32 slot, UInt64 offset)
	{
		D3D12_GPU_VIRTUAL_ADDRESS address = buffer->GetGPUAddress();
		address += offset;

		m_list->SetGraphicsRootConstantBufferView(slot, address);
	}

	void D3D12CommandBuffer::SetComputeCBV(RHIBuffer* buffer, UInt32 slot, UInt64 offset)
	{
		D3D12_GPU_VIRTUAL_ADDRESS address = buffer->GetGPUAddress();
		address += offset;

		m_list->SetComputeRootConstantBufferView(slot, address);
	}

	void D3D12CommandBuffer::TransitionBarrier(RHIResource** resources, UInt32 resource_num, ERHIResourceState after)
	{
		D3D12_RESOURCE_BARRIER barriers[MaxTranstionCount] = {};
		UInt32 resource_count = 0;
		for (UInt32 resource_index = 0; resource_index < resource_num; ++resource_index)
		{
			auto resource = resources[resource_index];
			auto before = resource->GetCurrentState();
			if (before == after)
				continue;
			// GenericReadへの遷移はサポートしない
			if (before == ERHIResourceState::GenericRead || after == ERHIResourceState::GenericRead)
			{
				
				continue;
			}

			auto native_resource = static_cast<ID3D12Resource1*>(resource->GetNative());
			barriers[resource_index].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barriers[resource_index].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barriers[resource_index].Transition.pResource = native_resource;
			barriers[resource_index].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barriers[resource_index].Transition.StateBefore = GetD3D12ResourceStates(before);
			barriers[resource_index].Transition.StateAfter = GetD3D12ResourceStates(after);
			// 状態更新
			resource->SetState(after);

			resource_count++;
			if (resource_count > MaxTranstionCount)
				break;

		}

		if (resource_count > 0)
			m_list->ResourceBarrier(resource_count, barriers);
	}

	void D3D12CommandBuffer::UAVBarrier(RHIBuffer** buffers, UInt32 buffer_num)
	{
		D3D12_RESOURCE_BARRIER barriers[MaxTranstionCount] = {};

		// pix用のやつとか埋めたいね

		for (UInt32 buffer_index = 0; buffer_index < buffer_num; ++buffer_index)
		{
			auto buffer = buffers[buffer_index];
			auto native_resource = static_cast<ID3D12Resource1*>(buffer->GetNative());
			barriers[buffer_index].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barriers[buffer_index].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barriers[buffer_index].UAV.pResource = native_resource;
		}

		m_list->ResourceBarrier(buffer_num, barriers);
	}

	void D3D12CommandBuffer::UAVBarrier(RHITexture** textures, UInt32 texture_num)
	{
		D3D12_RESOURCE_BARRIER barriers[MaxTranstionCount] = {};

		for (UInt32 texture_index = 0; texture_index < texture_num; ++texture_index)
		{
			auto texture = textures[texture_index];
			auto native_resource = static_cast<ID3D12Resource1*>(texture->GetNative());
			
			barriers[texture_index].Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
			barriers[texture_index].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barriers[texture_index].UAV.pResource = native_resource;
		}

		m_list->ResourceBarrier(texture_num, barriers);
	}

	void D3D12CommandBuffer::SetRenderTarget(RHIDescriptorHeap* rtv_heap, TFixedArray<UInt32, 8> rtv_indices, UInt32 rtv_count, RHIDescriptorHeap* dsv_heap, UInt32 dsv_index)
	{
		auto d3d12_rtv_heap = static_cast<D3D12DescriptorHeap*>(rtv_heap);

		// D3D12の最大値「8」で固定長配列を確保
		D3D12_CPU_DESCRIPTOR_HANDLE rtv_handles[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

		for (UInt32 i = 0; i < rtv_count; ++i)
		{
			rtv_handles[i] = d3d12_rtv_heap->GetCPUHandle(rtv_indices[i]);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = {};
		D3D12_CPU_DESCRIPTOR_HANDLE* p_dsv_handle = nullptr;
		if (dsv_heap)
		{
			auto d3d12_dsv_heap = static_cast<D3D12DescriptorHeap*>(dsv_heap);
			dsv_handle = d3d12_dsv_heap->GetCPUHandle(dsv_index);
			p_dsv_handle = &dsv_handle;
		}

		m_list->OMSetRenderTargets(rtv_count, rtv_handles, false, p_dsv_handle);
	}

	void D3D12CommandBuffer::ClearRenderTarget(RHIDescriptorHeap* rtv_heap, UInt32 rtv_index, const Rect2D& rect, const float* clear_color)
	{
		auto d3d12_rtv_heap = static_cast<D3D12DescriptorHeap*>(rtv_heap);

		D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = d3d12_rtv_heap->GetCPUHandle(rtv_index);

		D3D12_RECT d3d_rect = {};
		d3d_rect.left = (LONG)rect.x;
		d3d_rect.top = (LONG)rect.y;
		d3d_rect.right = (LONG)(rect.x + rect.width);
		d3d_rect.bottom = (LONG)(rect.y + rect.height);

		m_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
	}

	void D3D12CommandBuffer::ClearDepthStencil(RHIDescriptorHeap* dsv_heap, UInt32 dsv_index, const Rect2D& rect, float depth, uint8_t stencil)
	{
		auto d3d12_dsv_heap = static_cast<D3D12DescriptorHeap*>(dsv_heap);

		D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = d3d12_dsv_heap->GetCPUHandle(dsv_index);

		D3D12_RECT d3d_rect = {};
		d3d_rect.left = (LONG)rect.x;
		d3d_rect.top = (LONG)rect.y;
		d3d_rect.right = (LONG)(rect.x + rect.width);
		d3d_rect.bottom = (LONG)(rect.y + rect.height);

		m_list->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	}

	void D3D12CommandBuffer::ClearUnorderedAccess(RHIDescriptorHeap* uav_heap, UInt32 uav_index, RHITexture* texture, const float* clear_values)
	{
		auto d3d12_uav_heap = static_cast<D3D12DescriptorHeap*>(uav_heap);

		auto native_resource = static_cast<ID3D12Resource1*>(texture->GetNative());

		D3D12_CPU_DESCRIPTOR_HANDLE uav_cpu_handle = d3d12_uav_heap->GetCPUHandle(uav_index);
		D3D12_GPU_DESCRIPTOR_HANDLE uav_gpu_handle = d3d12_uav_heap->GetGPUHandle(uav_index);

		m_list->ClearUnorderedAccessViewFloat(uav_gpu_handle, uav_cpu_handle, native_resource, clear_values, 0, nullptr);
	}

	void D3D12CommandBuffer::ClearUnorderedAccess(RHIDescriptorHeap* uav_heap, UInt32 uav_index, RHIBuffer* buffer)
	{
		auto d3d12_uav_heap = static_cast<D3D12DescriptorHeap*>(uav_heap);

		auto native_resource = static_cast<ID3D12Resource1*>(buffer->GetNative());

		D3D12_CPU_DESCRIPTOR_HANDLE uav_cpu_handle = d3d12_uav_heap->GetCPUHandle(uav_index);
		D3D12_GPU_DESCRIPTOR_HANDLE uav_gpu_handle = d3d12_uav_heap->GetGPUHandle(uav_index);


		UINT clear_values[4] = { 0,0,0,0 };

		m_list->ClearUnorderedAccessViewUint(uav_gpu_handle, uav_cpu_handle, native_resource, clear_values, 0, nullptr);
	}

	void D3D12CommandBuffer::SetViewport(const Viewport& viewport_)
	{
		D3D12_VIEWPORT viewport = {};
		viewport.TopLeftX = viewport_.x;
		viewport.TopLeftY = viewport_.y;
		viewport.Width = viewport_.width;
		viewport.Height = viewport_.height;
		viewport.MinDepth = viewport_.min_depth;
		viewport.MaxDepth = viewport_.max_depth;
		m_list->RSSetViewports(1, &viewport);

	}

	void D3D12CommandBuffer::SetScissorRect(const Rect2D& rect)
	{
		D3D12_RECT scissor = {};
		scissor.left = (LONG)rect.x;
		scissor.right = (LONG)rect.x + rect.width;
		scissor.top = (LONG)rect.y;
		scissor.bottom = (LONG)rect.y + rect.height;
		m_list->RSSetScissorRects(1, &scissor);
	}

	void D3D12CommandBuffer::Draw(UInt32 vertex_count, UInt32 instance_count, UInt32 start_vertex, UInt32 start_instance)
	{
		m_list->DrawInstanced(vertex_count, instance_count, start_vertex, start_instance);
	}

	void D3D12CommandBuffer::DrawIndexed(UInt32 index_count, UInt32 start_index, int32_t base_vertex)
	{
		m_list->DrawIndexedInstanced(index_count, 1, start_index, base_vertex, 0);
	}

	void D3D12CommandBuffer::Dispatch(UInt32 thread_group_x, UInt32 thread_group_y, UInt32 thread_group_z)
	{
		m_list->Dispatch(thread_group_x, thread_group_y, thread_group_z);
	}

	void D3D12CommandBuffer::DispatchMesh(UInt32 thread_group_x, UInt32 thread_group_y, UInt32 thread_group_z)
	{
		m_list->DispatchMesh(thread_group_x, thread_group_y, thread_group_z);
	}

	void D3D12CommandBuffer::ExecuteIndirect(RHICommandLayout* layout, RHIBuffer* command_buffer, UInt32 count)
	{
		auto d3d12_layout = static_cast<D3D12CommandLayout*>(layout);
		auto native_resource = static_cast<ID3D12Resource1*>(command_buffer->GetNative());

		m_list->ExecuteIndirect(d3d12_layout->GetNativeHandle(), count, native_resource, 0, nullptr, 0);
	}

	void D3D12CommandBuffer::ExecuteIndirect(RHICommandLayout* layout, RHIBuffer* command_buffer, RHIBuffer* count_buffer)
	{
		// TODO
	}

	void D3D12CommandBuffer::DispatchRayTrace(const RHIRayTraceDesc& desc)
	{
		D3D12_DISPATCH_RAYS_DESC rayDesc = {};

		rayDesc.Width = desc.width;
		rayDesc.Height = desc.height;
		rayDesc.Depth = desc.depth;

		D3D12_GPU_VIRTUAL_ADDRESS shader_table_address = desc.shader_table_buffer->GetGPUAddress();

		rayDesc.RayGenerationShaderRecord.StartAddress = shader_table_address + desc.ray_gen_offset;
		rayDesc.RayGenerationShaderRecord.SizeInBytes = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;

		rayDesc.MissShaderTable.StartAddress = shader_table_address + desc.miss_offset;
		rayDesc.MissShaderTable.SizeInBytes = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT * desc.miss_count;
		rayDesc.MissShaderTable.StrideInBytes = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;

		rayDesc.HitGroupTable.StartAddress = shader_table_address + desc.hit_group_offset;
		rayDesc.HitGroupTable.SizeInBytes = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT * desc.hit_group_count;
		rayDesc.HitGroupTable.StrideInBytes = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;

		// 今は無視
		rayDesc.CallableShaderTable;

		m_list->DispatchRays(&rayDesc);

	}

	void D3D12CommandBuffer::BuildBLAS(const RHIBuildBLASDesc& desc)
	{
		UInt32 geometry_count = desc.geometries.size();
		TArray<D3D12_RAYTRACING_GEOMETRY_DESC> geometry_desc(geometry_count);

		bool is_traiangle = (desc.vertex_buffer);

		if (is_traiangle)
		{

			D3D12_GPU_VIRTUAL_ADDRESS vertex_address = desc.vertex_buffer->GetGPUAddress() + desc.vertex_offset;
			D3D12_GPU_VIRTUAL_ADDRESS index_address = desc.index_buffer->GetGPUAddress() + desc.index_offset;


			for (UInt32 i = 0; i < geometry_count; ++i)
			{
				auto& geometry = geometry_desc[i];
				geometry.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
				geometry.Flags = (desc.geometries[i].is_opaque) ? D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE : D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;

				geometry.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
				geometry.Triangles.VertexCount = desc.vertex_count;

				geometry.Triangles.VertexBuffer.StartAddress = vertex_address;
				geometry.Triangles.VertexBuffer.StrideInBytes = sizeof(Vector3);

				geometry.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
				geometry.Triangles.IndexCount = desc.geometries[i].index_count;

				geometry.Triangles.IndexBuffer = index_address + desc.geometries[i].index_byte_offset;

			}
		}
		else
		{

			D3D12_GPU_VIRTUAL_ADDRESS procedural_address = desc.procedural_buffer->GetGPUAddress() + desc.procedural_offset;

			for (UInt32 i = 0; i < geometry_count; ++i)
			{
				auto& geometry = geometry_desc[i];
				geometry.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
				geometry.Flags = (desc.geometries[i].is_opaque) ? D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE : D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;

				geometry.AABBs.AABBCount = desc.geometries[i].aabb_count;
				geometry.AABBs.AABBs.StartAddress = procedural_address;
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
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		inputs.Flags = flags;
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.NumDescs = geometry_count;
		inputs.pGeometryDescs = geometry_desc.data();

		D3D12_GPU_VIRTUAL_ADDRESS result_as_address = desc.result_as_buffer->GetGPUAddress() + desc.result_as_offset;
		D3D12_GPU_VIRTUAL_ADDRESS scratcht_as_address = desc.scratch_as_buffer->GetGPUAddress() + desc.scratch_as_offset;

		// 
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC as_desc = {};
		as_desc.Inputs = inputs;
		as_desc.DestAccelerationStructureData = result_as_address;
		as_desc.ScratchAccelerationStructureData = scratcht_as_address;

		if (desc.source_as_buffer)
		{
			D3D12_GPU_VIRTUAL_ADDRESS source_as_address = desc.source_as_buffer->GetGPUAddress() + desc.source_as_offset;

			as_desc.SourceAccelerationStructureData = source_as_address;
			as_desc.Inputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
		}


		m_list->BuildRaytracingAccelerationStructure(&as_desc, 0, nullptr);
	}

	void D3D12CommandBuffer::BuildTLAS(const RHIBuildTLASDesc& desc)
	{
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};
		inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		inputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE;
		inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		inputs.NumDescs = desc.instance_count;
		inputs.InstanceDescs = desc.instance_desc_buffer->GetGPUAddress();

		D3D12_GPU_VIRTUAL_ADDRESS result_as_address = desc.result_as_buffer->GetGPUAddress();
		D3D12_GPU_VIRTUAL_ADDRESS scratcht_as_address = desc.scratch_as_buffer->GetGPUAddress();

		// 
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC as_desc = {};
		as_desc.Inputs = inputs;
		as_desc.DestAccelerationStructureData = result_as_address;
		as_desc.ScratchAccelerationStructureData = scratcht_as_address;

		m_list->BuildRaytracingAccelerationStructure(&as_desc, 0, nullptr);

	}

	void D3D12CommandBuffer::Close()
	{
		m_list->Close();
	}

	void D3D12CommandBuffer::Reset()
	{
		//m_list->Reset(nullptr, nullptr);
	}
}

#endif