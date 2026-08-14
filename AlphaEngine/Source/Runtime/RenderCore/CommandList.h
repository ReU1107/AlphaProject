#pragma once
#include "Core/Common.h"
#include "RHI/Types.h"
#include "RHI/Definitions.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class RHICommandBuffer;

	class PipelineLayout;
	class GraphicsPipeline;
	class ComputePipeline;
	class RayTracePipeline;

	class DescriptorHeap;
	struct DescriptorHandle;

	class GPUResource;
	class Buffer;
	class Texture;
	class SamplerState;

	struct RayTraceDispatchArgs;


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
			UInt32 buffer_num{ 3 };
			// 追加の初期化パラメーターをここに記述できます
		};

		void Initialize(const Args& args);

		RHICommandBuffer* GetRHI() const;

		void Begin(UInt32 current_index);

		void End();

		void SetDescriptorHeap(DescriptorHeap* srv_uav_cbv_heap, DescriptorHeap* sampler_heap = nullptr);

		void SetViewport(const Viewport& viewport);

		void SetScissorRect(int32_t x, int32_t y, UInt32 width, UInt32 height);

		void SetPipelineLayout(PipelineLayout* layout);

		void SetGraphicsPipeline(GraphicsPipeline* pipeline);
		void SetComputePipeline(ComputePipeline* pipeline);
		void SetRayTracePipeline(RayTracePipeline* pipeline);

		void SetRenderTarget(Texture* render_target, Texture* depth_stencil);
		void SetRenderTargets(Texture** render_targets, UInt32 num, Texture* depth_stencil);

		void ClearRenderTarget(Texture* render_target, const Rect2D& rect, const Color4& clear_color);
		void ClearRenderTargets(Texture** render_targets, UInt32 num, const Rect2D& rect, const Color4& clear_color);

		void ClearDepthStencil(Texture* depth_stencil, const Rect2D& rect, float clear_depth);

		void SetPrimitiveTopology(ERHIPrimitiveTopology topology);

		void SetVertexBuffer(Buffer* rb, UInt32 slot);

		void SetIndexBuffer(Buffer* rb, ERHIIndexFormat format);

		// こいつらはバインドレス経由じゃなく従来の方法のパターン用
		void SetGraphicsTexture(Texture* texture, UInt32 slot);
		void SetGraphicsSamplerState(SamplerState* state, UInt32 slot);
		void SetComputeSamplerState(SamplerState* state, UInt32 slot);

		void SetGraphcisSRV(Buffer* buffer, UInt32 slot);
		void SetComputeSRV(Buffer* buffer, UInt32 slot);
		void SetGraphcisSRV(Buffer* buffer, UInt64 offset, UInt32 slot);

		void SetGraphcisUAV(Buffer* buffer, UInt32 slot);
		void SetComputeUAV(Buffer* buffer, UInt32 slot);

		void SetGraphicsCBV(Buffer* buffer, UInt32 slot);
		void SetGraphicsCBV(Buffer* buffer, UInt64 offset, UInt32 slot);
		void SetComputeCBV(Buffer* buffer, UInt64 offset, UInt32 slot);

		void Transition(GPUResource* resource, ERHIResourceState after);
		void Transition(Texture** textures, UInt32 num, ERHIResourceState after);

		void CopyBuffer(Buffer* src, Buffer* dst);
		void CopyBufferRegion(Buffer* src, UInt64 src_offset, Buffer* dst, UInt64 dst_offset, UInt64 size);

		void CopyTexture(Texture* src, Texture* dst);
		void CopyTextureRegion(Buffer* src, UInt64 src_offset, Texture* dst);
		//void CopyTextureRegion(Texture* src, const RHIRect& src_rect, Texture* dst, const RHIRect& dst_rect);

		void UAVBarrier(Buffer* buffer);
		void UAVBarrier(Texture* texture);
		void UAVBarrier(Texture** textures, UInt32 num);

		void Draw(UInt32 vertex_count, UInt32 instance_count, UInt32 start_vertex, UInt32 start_instance);

		void DrawIndexed(UInt32 index_count, UInt32 start_index, int32_t base_vertex);

		void Dispatch(UInt32 group_count_x, UInt32 group_count_y, UInt32 group_count_z);
		void DispatchMesh(UInt32 group_count_x, UInt32 group_count_y, UInt32 group_count_z);
		void DispatchRayTrace(const RayTraceDispatchArgs& desc);


		void BuildRaytracingAccelerationStructure(const RHIBuildTLASDesc& desc);
		void BuildRaytracingAccelerationStructure(const RHIBuildBLASDesc& desc);

		void SetName(StringView name);
	};

}