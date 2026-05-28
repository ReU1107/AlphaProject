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
	
	class DescriptorTable;
	class DescriptorHeap;
	
	class GPUResource;
	class Buffer;
	class StructuredBuffer;
	class ConstantBuffer;
	class ByteAddressBuffer;
	class RingBuffer;
	
	class Texture;
	class RenderTexture;
	class DepthTexture;

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

		TUnique<DescriptorHeap> m_bindless_heap{};
		TUnique<DescriptorHeap> m_swap_chain_rtv{};
		TUnique<DescriptorHeap> m_swap_chain_dsv{};
		TUnique<DescriptorHeap> m_sampler_heap{};

		TUniqueArray<RenderTexture> m_back_buffer{};
		TUnique<DepthTexture> m_depth_texture{};

		TUniqueArray<SamplerState> m_preset_samplers{};

		TUnique<GPUDataTransfer> m_gpu_data_transfer{};
		TUnique<RingBuffer> m_transient_buffer{};

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

			uint64_t transient_buffer_size{ 1_MB };
			UInt32 bindless_num{ 4096 };

			UInt32 back_buffer_count{ 3 };

		};

		bool Initialize(const Args& args);

		void Finalize();

		TUnique<CommandQueue> CreateCommandQueue(ERHICommandType type);
		TUnique<CommandList> CreateCommandList(ERHICommandType type);
		TUnique<Timeline> CreateTimeline();

		TUnique<DescriptorHeap> CreateDescriptorHeap(uint64_t num, ERHIDescriptorType type);

		TUnique<ConstantBuffer> CreateConstantBuffer(uint64_t stride, uint64_t element_num, void* data);
		TUnique<StructuredBuffer> CreateStructuredBuffer(uint64_t stride, uint64_t element_num, EResourceAccessFlags flags = EResourceAccessFlags::ReadOnly);
		TUnique<ByteAddressBuffer> CreateByteAddressBuffer(uint64_t element_num, EResourceAccessFlags flags = EResourceAccessFlags::ReadOnly);
		TUnique<RingBuffer> CreateRingBuffer(uint64_t size);
		TUnique<RingBuffer> CreateRingBuffer(uint64_t stride, uint64_t element_num);

		TUnique<SamplerState> CreateSamplerState(ERHIFilter filter, ERHIAddressMode address);

		TUnique<Texture> CreateTexture(UInt32 width, UInt32 height, ERHIResourceFormat format, void* data);
		TUnique<RenderTexture> CreateRenderTexture(DescriptorHeap* heap, UInt32 width, UInt32 height, ERHIResourceFormat format);
		TUnique<DepthTexture> CreateDepthTexture(DescriptorHeap* heap, UInt32 width, UInt32 height, ERHIResourceFormat format);

		TUnique<Shader> CreateShader(const std::wstring& file_path, ERHIShaderType shader_type);

		TUnique<DescriptorTable> CreateDescriptorTable(DescriptorHeap* heap, std::function<void(DescriptorTable&)> config_func);

		TUnique<PipelineLayout> CreatePipelineLayout(std::function<void(PipelineLayout&)> config_func);

		TUnique<GraphicsPipeline> CreateGraphicsPipeline(GraphicsPipelineArgs args);

		TUnique<ComputePipeline> CreateComputePipeline(ComputePipelineArgs args);

		ERHIResourceFormat GetBackBufferFormat() const { return m_back_buffer_format; }

		RenderTexture* GetCurrentRenderTarget() const;
		DepthTexture* GetDepthTexture() const;

		CommandList* GetCommandList() const { return m_command_list.get(); }
		CommandQueue* GetCommandQueue() const { return m_command_queue.get(); }
		Timeline* GetTimeline() const { return m_timeline.get(); }

		DescriptorHeap* GetBindlessHeap() const { return m_bindless_heap.get(); }
		DescriptorHeap* GetSamplerHeap() const { return m_sampler_heap.get(); }

		SamplerState* GetPresetSampler(ESamplerPresetType type) { return m_preset_samplers[+type].get(); }

		RingBuffer* GetTransientBuffer() const { return m_transient_buffer.get(); }

		UInt32 GetWidth() const { return m_width; }
		UInt32 GetHeight() const { return m_height; }

		RenderHardwareInterface* GetRHI() const { return m_rhi.get(); }

		bool PostInitialize();

		void BegineFrame();

		void EndFrame();
	};
}
