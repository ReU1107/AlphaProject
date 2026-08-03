#pragma once
#include "Core/Common.h"
#include "Types.h"
#include "Declarations.h"

namespace Alpha
{
	// 複数のコマンドバッファオブジェクトを持つ(バッファリング)
	class RHICommandBuffer
	{
	public:
		virtual ~RHICommandBuffer(){}

		virtual bool Initialize(const RHICommandBufferDesc& desc) = 0;

		virtual void SetName(StringView name) = 0;

		virtual void Begin(UInt32) = 0;

		virtual void End() = 0;

		virtual void SetGraphicsPipelineLayout(RHIPipelineLayout* layout) = 0;
		virtual void SetComputePipelineLayout(RHIPipelineLayout* layout) = 0;

		virtual void SetGraphicsPipeline(RHIGraphicsPipeline* pipeline, RHIPipelineLayout* layout) = 0;
		virtual void SetComputePipeline(RHIComputePipeline* pipeline, RHIPipelineLayout* layout) = 0;
		virtual void SetRayTracePipeline(RHIRayTracePipeline* pipeline, RHIPipelineLayout* layout) = 0;

		virtual void SetDescriptorHeap(RHIDescriptorHeap* heap) = 0;
		virtual void SetDescriptorHeap(RHIDescriptorHeap* srv_heap, RHIDescriptorHeap* sampler_heap) = 0;

		virtual void SetRenderTarget(RHIDescriptorHeap* rtv_heap, TFixedArray<UInt32, 8> rtv_indices, UInt32 rtv_count, RHIDescriptorHeap* dsv_heap, UInt32 dsv_index) = 0;

		virtual void ClearRenderTarget(RHIDescriptorHeap* rtv_heap, UInt32 rtv_index, const Rect2D& rect, const float* clear_color) = 0;
		virtual void ClearDepthStencil(RHIDescriptorHeap* dsv_heap, UInt32 dsv_index, const Rect2D& rect, float depth, uint8_t stencil) = 0;
		virtual void CopyTexture(RHITexture* src, RHITexture* dst) = 0;
		virtual void CopyTextureRegion(RHIBuffer* src, UInt64 src_offset, RHITexture* dst) = 0;

		virtual void CopyBuffer(RHIBuffer* src, RHIBuffer* dst) = 0;
		virtual void CopyBufferRegion(RHIBuffer* src, UInt64 src_offset, RHIBuffer* dst, UInt64 dst_offset, UInt64 size) = 0;

		virtual void SetVertexBuffer(RHIBuffer* vb, UInt32 slot) = 0;

		virtual void SetPrimitiveTopology(ERHIPrimitiveTopology topology) = 0;

		virtual void SetIndexBuffer(RHIBuffer* ib, ERHIIndexFormat format) = 0;

		virtual void SetUniformBuffer(RHIBuffer* cb, UInt32 slot, ERHIPipelineType type = ERHIPipelineType::Graphics) = 0;

		virtual void SetGraphicsTable(RHIDescriptorHeap* view, UInt32 index, UInt32 slot) = 0;
		virtual void SetComputeTable(RHIDescriptorHeap* view, UInt32 index, UInt32 slot) = 0;

		virtual void SetGraphicsUAV(RHIBuffer* buffer, UInt32 slot) = 0;
		virtual void SetComputeUAV(RHIBuffer* buffer, UInt32 slot) = 0;

		virtual void SetGraphicsSRV(RHIBuffer* buffer, UInt32 slot, UInt64 offset) = 0;
		virtual void SetComputeSRV(RHIBuffer* buffer, UInt32 slot, UInt64 offset) = 0;

		virtual void SetGraphicsCBV(RHIBuffer* buffer, UInt32 slot, UInt64 offset) = 0;
		virtual void SetComputeCBV(RHIBuffer* buffer, UInt32 slot, UInt64 offset) = 0;

		virtual void SetViewport(const Viewport& viewport) = 0;
		virtual void SetScissorRect(const Rect2D& rect) = 0;

		virtual void TransitionBarrier(RHIResource** resource, UInt32 resource_num, ERHIResourceState after) = 0;

		virtual void UAVBarrier(RHIBuffer** buffers, UInt32 buffer_count) = 0;
		virtual void UAVBarrier(RHITexture** textures, UInt32 texture_count) = 0;

		virtual void Draw(UInt32 vertex_count, UInt32 instance_count, UInt32 start_vertex, UInt32 start_instance) = 0;
		virtual void DrawIndexed(UInt32 index_count, UInt32 start_index, int32_t base_vertex) = 0;

		virtual void Dispatch(UInt32 thread_group_x, UInt32 thread_group_y, UInt32 thread_group_z) = 0;

		virtual void DispatchMesh(UInt32 thread_group_x, UInt32 thread_group_y, UInt32 thread_group_z) = 0;

		virtual void DispatchRayTrace(const RHIRayTraceDesc& desc) = 0;

		virtual void BuildBLAS(const RHIBuildBLASDesc& desc) = 0;
		virtual void BuildTLAS(const RHIBuildTLASDesc& desc) = 0;

		virtual void Close() = 0;
		virtual void Reset() = 0;
	};

}