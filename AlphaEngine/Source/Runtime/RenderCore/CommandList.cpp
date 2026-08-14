#include "CommandList.h"
#include "RHI/RHICommandBuffer.h"
#include "RenderHardwareInterface.h"

#include "DescriptorHeap.h"
#include "DescriptorHandle.h"

#include "SamplerState.h"

#include "Texture.h"
#include "Buffer.h"
#include "RHI/RHITexture.h"

#include "GraphicsPipeline.h"
#include "ComputePipeline.h"
#include "RayTracePipeline.h"
#include "PipelineLayout.h"

#include "Definitions.h"

namespace Alpha
{
	CommandList::CommandList() = default;

	CommandList::~CommandList() = default;

	void CommandList::Initialize(const Args& args)
	{
		RHICommandBufferDesc desc;
		desc.type = args.type;
		desc.count = args.buffer_num;
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

	void CommandList::SetPipelineLayout(PipelineLayout* layout)
	{
		m_rhi->SetGraphicsPipelineLayout(layout->GetRHI());
		m_rhi->SetComputePipelineLayout(layout->GetRHI());
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

	void CommandList::SetRayTracePipeline(RayTracePipeline* pipeline)
	{
		m_rhi->SetRayTracePipeline(pipeline->GetRHI(), pipeline->GetPipelineLayout()->GetRHI());
	}

	void CommandList::SetRenderTarget(Texture* render_target, Texture* depth_stencil)
	{
		SetRenderTargets(&render_target, 1, depth_stencil);
	}

	void CommandList::SetRenderTargets(Texture** render_targets, UInt32 num, Texture* depth_stencil)
	{
		ALPHA_CHECK_FATAL(num <= 8, L"レンダーターゲットの最大数は8までです");

		TFixedArray<UInt32, 8> rtv_indices{};

		RHIDescriptorHeap* rtv_heap = nullptr;

		for (UInt32 i = 0; i < num; ++i)
		{
			if (render_targets[i])
			{
				auto rtv_handle = render_targets[i]->GetRTVHandle();

				if (!rtv_heap)
				{
					rtv_heap = rtv_handle.heap->GetRHI();
				}

				rtv_indices[i] = rtv_handle.index;
			}
		}

		RHIDescriptorHeap* dsv_heap = nullptr;
		UInt32 dsv_index = 0;
		if (depth_stencil)
		{
			auto dsv_handle = depth_stencil->GetDSVHandle();
			dsv_heap = dsv_handle.heap->GetRHI();
			dsv_index = dsv_handle.index;
		}

		m_rhi->SetRenderTarget(rtv_heap, rtv_indices, num, dsv_heap, dsv_index);
	}

	void CommandList::ClearRenderTarget(Texture* render_target, const Rect2D& rect, const Color4& clear_color)
	{
		auto rtv_handle = render_target->GetRTVHandle();

		m_rhi->ClearRenderTarget(rtv_handle.heap->GetRHI(), rtv_handle.index, rect, clear_color.data());
	}

	void CommandList::ClearRenderTargets(Texture** render_targets, UInt32 num, const Rect2D& rect, const Color4& clear_color)
	{
		for (UInt32 i = 0; i < num; ++i)
		{
			ClearRenderTarget(render_targets[i], rect, clear_color);
		}
	}

	void CommandList::ClearDepthStencil(Texture* depth_stencil, const Rect2D& rect, float clear_depth)
	{
		auto dsv_handle = depth_stencil->GetDSVHandle();
		m_rhi->ClearDepthStencil(dsv_handle.heap->GetRHI(), dsv_handle.index, rect, clear_depth, 0);
	}
	
	void CommandList::SetPrimitiveTopology(ERHIPrimitiveTopology topology)
	{
		m_rhi->SetPrimitiveTopology(topology);
	}

	void CommandList::SetVertexBuffer(Buffer* rb, UInt32 slot)
	{
		m_rhi->SetVertexBuffer(rb->GetRHI(), slot);
	}

	void CommandList::SetIndexBuffer(Buffer* rb, ERHIIndexFormat format)
	{
		m_rhi->SetIndexBuffer(rb->GetRHI(), format);
	}

	void CommandList::SetGraphicsTexture(Texture* texture, UInt32 slot)
	{
		auto texture_handle = texture->GetSRVHandle();

		m_rhi->SetGraphicsTable(texture_handle.heap->GetRHI(), texture_handle.index, slot);
	}

	void CommandList::SetGraphicsSamplerState(SamplerState* state, UInt32 slot)
	{
		auto sampler_handle = state->GetSamplerHandle();

		m_rhi->SetGraphicsTable(sampler_handle.heap->GetRHI(), sampler_handle.index, slot);
	}

	void CommandList::SetComputeSamplerState(SamplerState* state, UInt32 slot)
	{
		auto sampler_handle = state->GetSamplerHandle();

		m_rhi->SetComputeTable(sampler_handle.heap->GetRHI(), sampler_handle.index, slot);
	}

	void CommandList::SetGraphcisSRV(Buffer* buffer, UInt32 slot)
	{
		m_rhi->SetGraphicsSRV(buffer->GetRHI(), slot, 0);
	}

	void CommandList::SetComputeSRV(Buffer* buffer, UInt32 slot)
	{
		m_rhi->SetComputeSRV(buffer->GetRHI(), slot, 0);
	}

	void CommandList::SetGraphcisSRV(Buffer* buffer, UInt64 offset, UInt32 slot)
	{
		m_rhi->SetGraphicsSRV(buffer->GetRHI(), slot, offset);
	}

	void CommandList::SetGraphcisUAV(Buffer* buffer, UInt32 slot)
	{
		m_rhi->SetGraphicsUAV(buffer->GetRHI(), slot);
	}

	void CommandList::SetComputeUAV(Buffer* buffer, UInt32 slot)
	{
		m_rhi->SetComputeUAV(buffer->GetRHI(), slot);
	}

	void CommandList::SetGraphicsCBV(Buffer* buffer, UInt32 slot)
	{
		m_rhi->SetGraphicsCBV(buffer->GetRHI(), slot, 0);
	}

	void CommandList::SetGraphicsCBV(Buffer* buffer, UInt64 offset, UInt32 slot)
	{
		m_rhi->SetGraphicsCBV(buffer->GetRHI(), slot, offset);
	}

	void CommandList::SetComputeCBV(Buffer* buffer, UInt64 offset, UInt32 slot)
	{
		m_rhi->SetComputeCBV(buffer->GetRHI(), slot, offset);
	}

	void CommandList::Transition(GPUResource* resource, ERHIResourceState after)
	{
		RHIResource* resources[] = { resource->GetRHI() };
		m_rhi->TransitionBarrier(resources, 1, after);
	}

	void CommandList::Transition(Texture** textures, UInt32 num, ERHIResourceState after)
	{
		RHIResource* rhi_resources[128] = {};

		for (UInt32 i = 0; i < num; ++i)
		{
			if (textures[i])
			{
				rhi_resources[i] = textures[i]->GetRHI();
			}
		}

		m_rhi->TransitionBarrier(rhi_resources, num, after);
	}

	void CommandList::CopyBuffer(Buffer* src, Buffer* dst)
	{
		m_rhi->CopyBuffer(src->GetRHI(), dst->GetRHI());
	}

	void CommandList::CopyBufferRegion(Buffer* src, UInt64 src_offset, Buffer* dst, UInt64 dst_offset, UInt64 size)
	{
		m_rhi->CopyBufferRegion(src->GetRHI(), src_offset, dst->GetRHI(), dst_offset, size);
	}

	void CommandList::CopyTexture(Texture* src, Texture* dst)
	{
	}

	void CommandList::CopyTextureRegion(Buffer* src, UInt64 src_offset, Texture* dst)
	{
		m_rhi->CopyTextureRegion(src->GetRHI(), src_offset, dst->GetRHI());
	}

	void CommandList::UAVBarrier(Buffer* buffer)
	{
		RHIBuffer* buffers[] = { buffer->GetRHI() };

		m_rhi->UAVBarrier(buffers, 1);
	}

	void CommandList::UAVBarrier(Texture* texture)
	{
		RHITexture* textures[] = { texture->GetRHI() };

		m_rhi->UAVBarrier(textures, 1);
	}

	void CommandList::UAVBarrier(Texture** textures, UInt32 num)
	{
		RHITexture* rhi_resources[128] = {};

		for (UInt32 i = 0; i < num; ++i)
		{
			if (textures[i])
			{
				rhi_resources[i] = textures[i]->GetRHI();
			}
		}

		m_rhi->UAVBarrier(rhi_resources, num);
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

	void CommandList::DispatchMesh(UInt32 group_count_x, UInt32 group_count_y, UInt32 group_count_z)
	{
		m_rhi->DispatchMesh(group_count_x, group_count_y, group_count_z);
	}

	void CommandList::DispatchRayTrace(const RayTraceDispatchArgs& args)
	{
		RHIRayTraceDesc desc = {};
		desc.shader_table_buffer = args.pipeline->GetShaderTableBuffer()->GetRHI();
		
		desc.ray_gen_offset = args.pipeline->GetRayGenerateTable().alloc.offset;
		
		desc.miss_offset = args.pipeline->GetMissTable().alloc.offset;
		desc.miss_count = args.pipeline->GetMissTable().count;

		desc.hit_group_offset = args.pipeline->GetHitGroupTable().alloc.offset;
		desc.hit_group_count = args.pipeline->GetHitGroupTable().count;


		desc.width = args.width;
		desc.height = args.height;
		desc.depth = args.depth;


		m_rhi->DispatchRayTrace(desc);

	}

	void CommandList::BuildRaytracingAccelerationStructure(const RHIBuildTLASDesc& desc)
	{
		m_rhi->BuildTLAS(desc);
	}

	void CommandList::BuildRaytracingAccelerationStructure(const RHIBuildBLASDesc& desc)
	{
		m_rhi->BuildBLAS(desc);
	}

	void CommandList::SetName(StringView name)
	{
		m_rhi->SetName(name);
	}
}

