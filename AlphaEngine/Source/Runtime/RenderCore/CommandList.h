#pragma once
#include "Core/Common.h"
#include "RHI/Types.h"
#include "RHI/Definitions.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class RHICommandBuffer;

	class GraphicsPipeline;
	class ComputePipeline;

	class DescriptorHeap;
	class DescriptorTable;
	struct DescriptorHandle;

	class GPUResource;
	class Buffer;
	class ByteAddressBuffer;
	class StructuredBuffer;
	class ConstantBuffer;
	class RingBuffer;
	class Texture;
	class RenderTexture;
	class DepthTexture;
	class SamplerState;


	class CommandList
	{
	private:
		TUnique<RHICommandBuffer> m_rhi{ nullptr };
	public:
		CommandList();
		~CommandList();

		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			ERHICommandType type{ ERHICommandType::Direct };
			// 追加の初期化パラメーターをここに記述できます
		};

		void Initialize(const Args& args);

		RHICommandBuffer* GetRHI() const;

		void Begin(UInt32 current_index);

		void End();

		void SetDescriptorHeap(DescriptorHeap* srv_uav_cbv_heap, DescriptorHeap* sampler_heap = nullptr);

		void SetViewport(const Viewport& viewport);

		void SetScissorRect(int32_t x, int32_t y, UInt32 width, UInt32 height);

		void SetGraphicsPipeline(GraphicsPipeline* pipeline);
		void SetComputePipeline(ComputePipeline* pipeline);

		void SetRenderTarget(RenderTexture* render_target, DepthTexture* depth_stencil);
		void SetRenderTargets(RenderTexture** render_targets, UInt32 num, DepthTexture* depth_stencil);

		void ClearRenderTarget(RenderTexture* render_target, const Rect2D& rect, const Color4& clear_color);

		void ClearDepthStencil(DepthTexture* depth_stencil, const Rect2D& rect, float clear_depth);

		void SetPrimitiveTopology(ERHIPrimitiveTopology topology);

		void SetVertexBuffer(RingBuffer* rb, UInt32 slot);

		void SetIndexBuffer(RingBuffer* rb, ERHIIndexFormat format);

		void SetGraphicsDescriptorTable(DescriptorTable* table, UInt32 slot);
		void SetGraphicsDescriptorTable(const DescriptorHandle& handle, UInt32 slot);
		void SetComputeDescriptorTable(DescriptorTable* table, UInt32 slot);

		void SetGraphicsSamplerState(SamplerState* state, UInt32 slot);

		void SetGraphcisSRV(Buffer* buffer, UInt32 slot);
		void SetComputeSRV(Buffer* buffer, UInt32 slot);
		void SetGraphcisSRV(RingBuffer* buffer, uint64_t offset, UInt32 slot);

		void SetGraphcisUAV(StructuredBuffer* buffer, UInt32 slot);
		void SetComputeUAV(StructuredBuffer* buffer, UInt32 slot);

		void SetGraphicsCBV(ConstantBuffer* buffer, UInt32 slot);
		void SetGraphicsCBV(RingBuffer* buffer, uint64_t offset, UInt32 slot);
		void SetComputeCBV(RingBuffer* buffer, uint64_t offset, UInt32 slot);

		void Transition(GPUResource* resource, ERHIResourceState after);

		void CopyBuffer(Buffer* src, Buffer* dst);
		void CopyBufferRegion(Buffer* src, uint64_t src_offset, Buffer* dst);

		void CopyTexture(Texture* src, Texture* dst);
		void CopyTextureRegion(Buffer* src, uint64_t src_offset, Texture* dst);
		//void CopyTextureRegion(Texture* src, const RHIRect& src_rect, Texture* dst, const RHIRect& dst_rect);

		void UAVBarrier(Buffer* buffer);

		void Draw(UInt32 vertex_count, UInt32 instance_count, UInt32 start_vertex, UInt32 start_instance);

		void DrawIndexed(UInt32 index_count, UInt32 start_index, int32_t base_vertex);

		void Dispatch(UInt32 group_count_x, UInt32 group_count_y, UInt32 group_count_z);

	};

}