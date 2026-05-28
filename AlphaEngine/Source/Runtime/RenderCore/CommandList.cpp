#include "CommandList.h"
#include "RHI/RHICommandBuffer.h"
#include "RenderHardwareInterface.h"

#include "DescriptorHeap.h"
#include "DescriptorTable.h"
#include "DescriptorHandle.h"

#include "SamplerState.h"

#include "Texture/Texture.h"
#include "Texture/RenderTexture.h"
#include "Texture/DepthTexture.h"

#include "Buffer/ConstantBuffer.h"
#include "Buffer/StructuredBuffer.h"
#include "Buffer/ByteAddressBuffer.h"
#include "Buffer/RingBuffer.h"

#include "Pipeline/GraphicsPipeline.h"
#include "Pipeline/ComputePipeline.h"
#include "Pipeline/PipelineLayout.h"

namespace Alpha
{
	CommandList::CommandList() = default;

	CommandList::~CommandList() = default;

	void CommandList::Initialize(const Args& args)
	{
		RHICommandBufferDesc desc;
		desc.type = args.type;
		desc.count = 3;
		m_rhi = args.rhi->CreateCommandBuffer(desc);
	}

	RHICommandBuffer* CommandList::GetRHI() const
	{
		return m_rhi.get();
	}

	void CommandList::Begin(UInt32 current_index)
	{
		m_rhi->Begin(current_index);
	}

	void CommandList::End()
	{
		m_rhi->End();
	}

	void CommandList::SetDescriptorHeap(DescriptorHeap* srv_uav_cbv_heap, DescriptorHeap* sampler_heap)
	{
		m_rhi->SetDescriptorHeap(srv_uav_cbv_heap->GetRHI(), sampler_heap->GetRHI());
	}

	void CommandList::SetViewport(const Viewport& viewport)
	{
		m_rhi->SetViewport(viewport);
	}

	void CommandList::SetScissorRect(int32_t x, int32_t y, UInt32 width, UInt32 height)
	{
		Rect2D rect{ (float)x, (float)y, (float)width, (float)height };
		m_rhi->SetScissorRect(rect);
	}

	void CommandList::SetGraphicsPipeline(GraphicsPipeline* pipeline)
	{
		m_rhi->SetGraphicsPipeline(pipeline->GetRHI(), pipeline->GetPipelineLayout()->GetRHI());

		m_rhi->SetPrimitiveTopology(pipeline->GetPrimitiveTopology());
	}

	void CommandList::SetComputePipeline(ComputePipeline* pipeline)
	{
		m_rhi->SetComputePipeline(pipeline->GetRHI(), pipeline->GetPipelineLayout()->GetRHI());
	}

	void CommandList::SetRenderTarget(RenderTexture* render_target, DepthTexture* depth_stencil)
	{
		auto rtv_handle = render_target->GetRTVHandle();

		RHIDescriptorHeap* dsv_heap = nullptr;
		UInt32 dsv_index = 0;
		if (depth_stencil)
		{
			auto dsv_handle = depth_stencil->GetDSVHandle();
			dsv_heap = dsv_handle.heap->GetRHI();
			dsv_index = dsv_handle.index;
		}
		
		m_rhi->SetRenderTarget(rtv_handle.heap->GetRHI(), rtv_handle.index, 1, dsv_heap, dsv_index);
	}

	void CommandList::SetRenderTargets(RenderTexture** render_targets, UInt32 num, DepthTexture* depth_stencil)
	{

	}

	void CommandList::ClearRenderTarget(RenderTexture* render_target, const Rect2D& rect, const Color4& clear_color)
	{
		auto rtv_handle = render_target->GetRTVHandle();

		m_rhi->ClearRenderTarget(rtv_handle.heap->GetRHI(), rtv_handle.index, rect, clear_color.data());
	}

	void CommandList::ClearDepthStencil(DepthTexture* depth_stencil, const Rect2D& rect, float clear_depth)
	{
		auto dsv_handle = depth_stencil->GetDSVHandle();
		m_rhi->ClearDepthStencil(dsv_handle.heap->GetRHI(), dsv_handle.index, rect, clear_depth, 0);
	}
	
	void CommandList::SetPrimitiveTopology(ERHIPrimitiveTopology topology)
	{
		m_rhi->SetPrimitiveTopology(topology);
	}

	void CommandList::SetVertexBuffer(RingBuffer* rb, UInt32 slot)
	{
		m_rhi->SetVertexBuffer(rb->GetRHI(), slot);
	}

	void CommandList::SetIndexBuffer(RingBuffer* rb, ERHIIndexFormat format)
	{
		m_rhi->SetIndexBuffer(rb->GetRHI(), format);
	}

	void CommandList::SetGraphicsDescriptorTable(DescriptorTable* table, UInt32 slot)
	{
		DescriptorHandle handle = table->GetHandle();
		m_rhi->SetGraphicsTable(handle.heap->GetRHI(), handle.index, slot);
	}

	void CommandList::SetGraphicsDescriptorTable(const DescriptorHandle& handle, UInt32 slot)
	{
		m_rhi->SetGraphicsTable(handle.heap->GetRHI(), handle.index, slot);
	}

	void CommandList::SetComputeDescriptorTable(DescriptorTable* table, UInt32 slot)
	{
		DescriptorHandle handle = table->GetHandle();
		m_rhi->SetComputeTable(handle.heap->GetRHI(), handle.index, slot);
	}

	void CommandList::SetGraphicsSamplerState(SamplerState* state, UInt32 slot)
	{
		SetGraphicsDescriptorTable(state->GetSamplerHandle(), slot);
	}

	void CommandList::SetGraphcisSRV(Buffer* buffer, UInt32 slot)
	{
		m_rhi->SetGraphicsSRV(buffer->GetRHI(), slot, 0);
	}

	void CommandList::SetComputeSRV(Buffer* buffer, UInt32 slot)
	{
		m_rhi->SetComputeSRV(buffer->GetRHI(), slot, 0);
	}

	void CommandList::SetGraphcisSRV(RingBuffer* buffer, uint64_t offset, UInt32 slot)
	{
		m_rhi->SetGraphicsSRV(buffer->GetRHI(), slot, offset);
	}

	void CommandList::SetGraphcisUAV(StructuredBuffer* buffer, UInt32 slot)
	{
		m_rhi->SetGraphicsUAV(buffer->GetRHI(), slot);
	}

	void CommandList::SetComputeUAV(StructuredBuffer* buffer, UInt32 slot)
	{
		m_rhi->SetComputeUAV(buffer->GetRHI(), slot);
	}

	void CommandList::SetGraphicsCBV(ConstantBuffer* buffer, UInt32 slot)
	{
		m_rhi->SetGraphicsCBV(buffer->GetRHI(), slot, 0);
	}

	void CommandList::SetGraphicsCBV(RingBuffer* buffer, uint64_t offset, UInt32 slot)
	{
		m_rhi->SetGraphicsCBV(buffer->GetRHI(), slot, offset);
	}

	void CommandList::SetComputeCBV(RingBuffer* buffer, uint64_t offset, UInt32 slot)
	{
		m_rhi->SetComputeCBV(buffer->GetRHI(), slot, offset);
	}

	void CommandList::Transition(GPUResource* resource, ERHIResourceState after)
	{
		RHIResource* resources[] = { resource->GetRHI() };
		m_rhi->TransitionBarrier(resources, 1, after);
	}

	void CommandList::CopyBuffer(Buffer* src, Buffer* dst)
	{
		m_rhi->CopyBuffer(src->GetRHI(), dst->GetRHI());
	}

	void CommandList::CopyBufferRegion(Buffer* src, uint64_t src_offset, Buffer* dst)
	{
		m_rhi->CopyBufferRegion(src->GetRHI(), src_offset, dst->GetRHI());
	}

	void CommandList::CopyTexture(Texture* src, Texture* dst)
	{
	}

	void CommandList::CopyTextureRegion(Buffer* src, uint64_t src_offset, Texture* dst)
	{
		m_rhi->CopyTextureRegion(src->GetRHI(), src_offset, dst->GetRHI());
	}

	void CommandList::UAVBarrier(Buffer* buffer)
	{
		RHIBuffer* buffers[] = { buffer->GetRHI() };

		m_rhi->UAVBarrier(buffers, 1);
	}

	void CommandList::Draw(UInt32 vertex_count, UInt32 instance_count, UInt32 start_vertex, UInt32 start_instance)
	{
		m_rhi->Draw(vertex_count, instance_count, start_vertex, start_instance);
	}
	
	void CommandList::DrawIndexed(UInt32 index_count, UInt32 start_index, int32_t base_vertex)
	{
		m_rhi->DrawIndexed(index_count, start_index, base_vertex);
	}

	void CommandList::Dispatch(UInt32 group_count_x, UInt32 group_count_y, UInt32 group_count_z)
	{
		m_rhi->Dispatch(group_count_x, group_count_y, group_count_z);
	}
}

