#pragma once
#include "Core/Subsystem.h"
#include "Core/Common.h"
#include "RHI/Types.h"
#include "Types.h"
#include "Pipeline/PipelineArgs.h"

#include <functional>

#include <Windows.h>

namespace Alpha
{
	class RenderHardwareInterface;
	
	class CommandQueue;
	class CommandList;
	class Timeline;
	
	class SwapChain;
	
	class DescriptorHeap;
	
	class GPUResource;
	class Buffer;
	class Texture;

	class SamplerState;
	
	class Shader;

	class PipelineLayout;
	class GraphicsPipeline;
	class ComputePipeline;

	class GPUDataTransfer;

	class RenderCore : public Subsystem<RenderCore>
	{
	private:
		TUnique<RenderHardwareInterface> m_rhi{ nullptr };
		TUnique<CommandQueue> m_command_queue{};
		TUnique<CommandList> m_command_list{};
		TUnique<Timeline> m_timeline{};

		TUnique<SwapChain> m_swap_chain{};

		TUnique<DescriptorHeap> m_resource_heap{};	// SRV_UAV_CBV
		TUnique<DescriptorHeap> m_rtv_heap{};
		TUnique<DescriptorHeap> m_dsv_heap{};
		TUnique<DescriptorHeap> m_sampler_heap{};

		TUniqueArray<Texture> m_back_buffer{};

		TUnique<Texture> m_depth_texture{};

		TUniqueArray<SamplerState> m_preset_samplers{};

		TUnique<GPUDataTransfer> m_gpu_data_transfer{};
		TUnique<Buffer> m_transient_buffer{};
		LinearAllocator m_transient_allocator{};

		UInt32 m_width{ 0 };
		UInt32 m_height{ 0 };

		ERHIResourceFormat m_back_buffer_format{ ERHIResourceFormat::Unknown };

	public:
		RenderCore();
		~RenderCore();

		struct Args
		{
			HWND hWnd{ nullptr };
			UInt32 width{ 1920 };
			UInt32 height{ 1080 };

			UInt64 transient_buffer_size{ 1_MB };
			UInt32 max_resource_num{ 1000000 };
			UInt32 max_sampler_num{ 2048 };
			UInt32 max_render_target_num{ 32 };
			UInt32 max_depth_stencil_num{ 16 };

			UInt32 back_buffer_count{ 3 };

		};

		bool Initialize(const Args& args);

		void Finalize();

		TUnique<CommandQueue> CreateCommandQueue(ERHICommandType type);
		TUnique<CommandList> CreateCommandList(ERHICommandType type);
		TUnique<Timeline> CreateTimeline();

		TUnique<DescriptorHeap> CreateDescriptorHeap(UInt64 num, ERHIDescriptorType type);

		TUnique<Buffer> CreateBuffer(UInt64 stride, UInt64 size, EBufferPresetType type);

		void UploadBuffer(Buffer* buffer, UInt64 offset, void* data, UInt64 data_size);

		TUnique<SamplerState> CreateSamplerState(ERHIFilter filter, ERHIAddressMode address);

		TUnique<Texture> CreateTexture(UInt32 width,
			UInt32 height,
			ERHIResourceBindFlags flags = ERHIResourceBindFlags::ShaderResource,
			ERHIResourceFormat format = ERHIResourceFormat::RGBAUnorm,
			ERHIResourceDimension dimension = ERHIResourceDimension::Tex2D,
			UInt32 array_size = 1);

		TUnique<Shader> CreateShader(const std::wstring& file_path, ERHIShaderType shader_type);

		TUnique<PipelineLayout> CreatePipelineLayout(std::function<void(PipelineLayout&)> config_func);

		TUnique<GraphicsPipeline> CreateGraphicsPipeline(GraphicsPipelineArgs args);

		TUnique<ComputePipeline> CreateComputePipeline(ComputePipelineArgs args);

		ERHIResourceFormat GetBackBufferFormat() const { return m_back_buffer_format; }

		Texture* GetCurrentRenderTarget() const;
		Texture* GetDepthTexture() const;

		CommandList* GetCommandList() const { return m_command_list.get(); }
		CommandQueue* GetCommandQueue() const { return m_command_queue.get(); }
		Timeline* GetTimeline() const { return m_timeline.get(); }

		DescriptorHeap* GetResourceHeap() const { return m_resource_heap.get(); }
		DescriptorHeap* GetSamplerHeap() const { return m_sampler_heap.get(); }

		SamplerState* GetPresetSampler(ESamplerPresetType type) { return m_preset_samplers[+type].get(); }

		Buffer* GetTransientBuffer() const { return m_transient_buffer.get(); }

		UInt32 GetWidth() const { return m_width; }
		UInt32 GetHeight() const { return m_height; }

		RenderHardwareInterface* GetRHI() const { return m_rhi.get(); }

		bool PostInitialize();

		void BegineFrame();

		void EndFrame();
	};
}
