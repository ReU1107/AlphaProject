#pragma once
#include "D3D12Device.h"
#include "RHI/RHICommandBuffer.h"
#include "Math/Rect.h"

#if RHI_D3D12


namespace Alpha
{
	class D3D12CommandBuffer : public RHICommandBuffer, public D3D12DeviceChild
	{
	private:

		TArray<ID3D12CommandAllocator*> m_allocators{};
		ComPtr<ID3D12GraphicsCommandList10> m_list{ nullptr };


	public:
		
		D3D12CommandBuffer(D3D12Device* device);
		~D3D12CommandBuffer();

		bool Initialize(const RHICommandBufferDesc& desc) final;

		ID3D12CommandList* GetNativeHandle() const { return m_list.Get(); }


		void CopyTexture(RHITexture* src, RHITexture* dst) final;
		void CopyTextureRegion(RHIBuffer* src, UInt64 src_offset, RHITexture* dst) final;

		void CopyBuffer(RHIBuffer* src, RHIBuffer* dst) final;
		void CopyBufferRegion(RHIBuffer* src, UInt64 src_offset, RHIBuffer* dst, UInt64 dst_offset, UInt64 size) final;

		void Begin(UInt32) final;

		void End() final;

		void SetPipelineLayout(RHIPipelineLayout* layout) final;

		void SetGraphicsPipeline(RHIGraphicsPipeline* pipeline, RHIPipelineLayout* layout) final;
		void SetComputePipeline(RHIComputePipeline* pipeline, RHIPipelineLayout* layout) final;

		void SetDescriptorHeap(RHIDescriptorHeap* heap) final;
		void SetDescriptorHeap(RHIDescriptorHeap* srv_heap, RHIDescriptorHeap* sampler_heap) final;

		void SetVertexBuffer(RHIBuffer* vb, UInt32 slot) final;

		void SetPrimitiveTopology(ERHIPrimitiveTopology topology) final;

		void SetIndexBuffer(RHIBuffer* ib, ERHIIndexFormat format) final;

		void SetUniformBuffer(RHIBuffer* cb, UInt32 slot, ERHIPipelineType type) final;

		void SetGraphicsTable(RHIDescriptorHeap* heap, UInt32 index, UInt32 slot) final;
		void SetComputeTable(RHIDescriptorHeap* heap, UInt32 index, UInt32 slot) final;

		void SetGraphicsUAV(RHIBuffer* buffer, UInt32 slot) final;
		void SetComputeUAV(RHIBuffer* buffer, UInt32 slot) final;

		void SetGraphicsSRV(RHIBuffer* buffer, UInt32 slot, UInt64 offset) final;
		void SetComputeSRV(RHIBuffer* buffer, UInt32 slot, UInt64 offset) final;

		void SetGraphicsCBV(RHIBuffer* buffer, UInt32 slot, UInt64 offset) final;
		void SetComputeCBV(RHIBuffer* buffer, UInt32 slot, UInt64 offset) final;

		void TransitionBarrier(RHIResource** resources, UInt32 resource_num, ERHIResourceState after) final;

		void UAVBarrier(RHIBuffer** buffers, UInt32 buffer_count) final;
		void UAVBarrier(RHITexture** textures, UInt32 texture_count) final;

		void SetRenderTarget(RHIDescriptorHeap* rtv_heap, UInt32 rtv_index, UInt32 rtv_count, RHIDescriptorHeap* dsv_heap, UInt32 dsv_index) final;

		void ClearRenderTarget(RHIDescriptorHeap* rtv_heap, UInt32 rtv_index, const Rect2D& rect, const float* clear_color) final;
		void ClearDepthStencil(RHIDescriptorHeap* dsv_heap, UInt32 dsv_index, const Rect2D& rect, float depth, uint8_t stencil) final;
		void SetViewport(const Viewport& viewport) final;
		void SetScissorRect(const Rect2D& rect) final;

		void Draw(UInt32 vertex_count, UInt32 instance_count, UInt32 start_vertex, UInt32 start_instance) final;
		void DrawIndexed(UInt32 index_count, UInt32 start_index, int32_t base_vertex) final;

		void Dispatch(UInt32 thread_group_x, UInt32 thread_group_y, UInt32 thread_group_z) final;

		void Close() final;
		void Reset() final;



		ID3D12GraphicsCommandList10* GetNative() const { return m_list.Get(); }
	};

}

#endif