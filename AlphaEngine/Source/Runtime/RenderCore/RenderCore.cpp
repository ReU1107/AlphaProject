#include "RenderCore.h"
#include "RenderHardwareInterface.h"

#include "SwapChain.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "Timeline.h"

#include "DescriptorHeap.h"

#include "Buffer.h"

#include "Texture.h"

#include "SamplerState.h"

#include "Shader.h"

#include "Pipeline/VertexLayout.h"
#include "Pipeline/PipelineLayout.h"
#include "Pipeline/GraphicsPipeline.h"
#include "Pipeline/ComputePipeline.h"

#include "GPUDataTransfer.h"
#include "TransientBufferAllocator.h"

#include "Core/Math/Vector.h"
#include "Core/Literal.h"

namespace Alpha
{
	RenderCore::RenderCore() = default;
	RenderCore::~RenderCore() = default;

	bool RenderCore::Initialize(const Args& args)
	{

		m_rhi = std::make_unique<RenderHardwareInterface>();
		RHI::Args rhi_args = {};
		m_rhi->Initialize(rhi_args);

		m_command_queue = CreateCommandQueue(L"RenderCore::GraphicsQueue", ERHICommandType::Direct);
		m_command_list = CreateCommandList(L"RenderCore::GraphicsList", ERHICommandType::Direct);

		m_swap_chain = std::make_unique<SwapChain>();
		{
			SwapChain::Args desc = {
				.rhi = m_rhi.get(),
				.queue = m_command_queue.get(),
				.window_handle = args.hWnd,
				.buffer_count = args.back_buffer_count,
				.width = args.width,
				.height = args.height,
			};
			m_swap_chain->Initialize(desc);
		}
		m_back_buffer_format = ERHIResourceFormat::RGBAUnorm;

		m_timeline = CreateTimeline();

		m_rtv_heap = CreateDescriptorHeap(args.max_render_target_num, ERHIDescriptorType::RTV);
		m_dsv_heap = CreateDescriptorHeap(args.max_depth_stencil_num, ERHIDescriptorType::DSV);

		// SwapChainのバッファ数分の RenderTextureを作成
		for (UInt32 i = 0; i < args.back_buffer_count; ++i)
		{
			auto render_texture = std::make_unique<Texture>();
			{
				Texture::Args desc = {
					.rhi = m_rhi.get(),
					.rtv_heap = m_rtv_heap.get(),
					.bind_flags = ERHIResourceBindFlags::RenderTarget,
					.swap_chain = m_swap_chain.get(),
					.buffer_index = i,
				};
				render_texture->Initialize(desc);
			}
			m_back_buffer.push_back(std::move(render_texture));
		}

		m_depth_texture = CreateTexture(args.width, args.height, ERHIResourceBindFlags::DepthStencil, ERHIResourceFormat::Depth24Stencil8);

		m_gpu_data_transfer = std::make_unique<GPUDataTransfer>();
		{
			GPUDataTransfer::Args desc = {
				.render_core = this,
				.staging_buffer_size = 64_MB,
			};
			m_gpu_data_transfer->Initialize(desc);
		}

		m_resource_heap = CreateDescriptorHeap(args.max_resource_num, ERHIDescriptorType::SRV_UAV_CBV);
		m_sampler_heap = CreateDescriptorHeap(args.max_sampler_num, ERHIDescriptorType::Sampler);

		m_preset_samplers.resize(+ESamplerPresetType::Count);

		m_preset_samplers[+ESamplerPresetType::PointClamp]	= CreateSamplerState(ERHIFilter::Point, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::LinearClamp]	= CreateSamplerState(ERHIFilter::Linear, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::LinearWrap]	= CreateSamplerState(ERHIFilter::Linear, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::AnisoWrap]	= CreateSamplerState(ERHIFilter::Anisotropic, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::Shadow]		= CreateSamplerState(ERHIFilter::ComparisonLinear, ERHIAddressMode::Border);

		m_transient_allocator = std::make_unique<TransientBufferAllocator>();
		{
			TransientBufferAllocator::Args desc = {
				.render_core = this,
				.block_num = args.back_buffer_count,
				.stride = 4,
				.size = args.transient_buffer_size,
				.alignment = 256,
			};
			m_transient_allocator->Initialize(desc);
		}

		m_width = args.width;
		m_height = args.height;		
		return true;
	}

	void RenderCore::Finalize()
	{
		if (m_timeline)
		{
			m_timeline->WaitCPU();
		}

		m_transient_allocator.reset();
		m_preset_samplers.clear();

		m_sampler_heap.reset();
		m_resource_heap.reset();

		m_gpu_data_transfer.reset();

		m_depth_texture.reset();

		for (auto& buffer : m_back_buffer)
		{
			buffer.reset();
		}
		m_back_buffer.clear();

		m_dsv_heap.reset();
		m_rtv_heap.reset();
		m_timeline.reset();

		m_swap_chain.reset();

		m_command_list.reset();
		m_command_queue.reset();

		if (m_rhi)
		{
			m_rhi->Finalize();
			m_rhi.reset();
		}
	}

	TUnique<CommandQueue> RenderCore::CreateCommandQueue(StringView name, ERHICommandType type)
	{
		auto queue = std::make_unique<CommandQueue>();
		CommandQueue::Args args = {
			.rhi = m_rhi.get(),
			.type = type,
		};
		queue->Initialize(args);
		queue->SetName(name);
		return queue;
	}

	TUnique<CommandList> RenderCore::CreateCommandList(StringView name, ERHICommandType type)
	{
		auto list = std::make_unique<CommandList>();
		CommandList::Args args = {
			.rhi = m_rhi.get(),
			.type = type,
		};
		list->Initialize(args);
		list->SetName(name);

		return list;
	}

	TUnique<Timeline> RenderCore::CreateTimeline()
	{
		auto timeline = std::make_unique<Timeline>();
		Timeline::Args desc = {
			.rhi = m_rhi.get(),
		};
		timeline->Initialize(desc);

		return timeline;
	}

	TUnique<DescriptorHeap> RenderCore::CreateDescriptorHeap(UInt64 num, ERHIDescriptorType type)
	{
		auto heap = std::make_unique<DescriptorHeap>();
		DescriptorHeap::Args args = {
			.rhi = m_rhi.get(),
			.type = type,
			.max_count = num,
		};
		heap->Initialize(args);

		return heap;
	}

	TUnique<Buffer> RenderCore::CreateBuffer(UInt64 stride, UInt64 size, EBufferPresetType type)
	{
		Buffer::Args desc = {
			.rhi = m_rhi.get(),
			.resource_heap = m_resource_heap.get(),
			.stride = stride,
			.size = size,
		};

		switch (type)
		{
		case EBufferPresetType::Constant:
			desc.access_type = ERHIResourceAccessType::Dynamic;
			desc.size = Alignment(desc.size, 256);

			break;
		case EBufferPresetType::Upload:
			desc.access_type = ERHIResourceAccessType::Dynamic;
			desc.size = Alignment(desc.size, 4);

			break;
		case EBufferPresetType::UploadStructured:
			desc.access_type = ERHIResourceAccessType::Dynamic;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;

			break;
		case EBufferPresetType::ROByteAddress:
			desc.access_type = ERHIResourceAccessType::Static;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;
			desc.bind_flags |= ERHIResourceBindFlags::Raw;
			desc.stride = 4; // ByteAddressBufferは4バイト固定
			break;
		case EBufferPresetType::ROStructured:
			desc.access_type = ERHIResourceAccessType::Static;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;

			break;
		case EBufferPresetType::RWByteAddress:
			desc.access_type = ERHIResourceAccessType::Static;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;
			desc.bind_flags |= ERHIResourceBindFlags::UnorderedAccess; // UAV追加
			desc.bind_flags |= ERHIResourceBindFlags::Raw;
			desc.stride = 4; // ByteAddressBufferは4バイト固定
			break;
		case EBufferPresetType::RWStructured:
			desc.access_type = ERHIResourceAccessType::Static;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;
			desc.bind_flags |= ERHIResourceBindFlags::UnorderedAccess; // UAV追加
			break;
		case EBufferPresetType::Readback:
			desc.access_type = ERHIResourceAccessType::Staging;
			break;
		default:
			ALPHA_LOG_INFO(L"未定義のバッファプリセットタイプが指定されました");
			break;
		}

		auto buffer = std::make_unique<Buffer>();
		buffer->Initialize(desc);

		return buffer;
	}

	void RenderCore::UploadBuffer(Buffer* buffer, UInt64 offset, const void* data, UInt64 data_size)
	{
		m_gpu_data_transfer->AddBufferRegion(buffer, offset, data, data_size);
	}

	TUnique<SamplerState> RenderCore::CreateSamplerState(ERHIFilter filter, ERHIAddressMode address)
	{
		auto sampler = std::make_unique<SamplerState>();

		SamplerState::Args args = {
			.rhi = m_rhi.get(),
			.sampler_heap = m_sampler_heap.get(),
			.filter = filter,
			.address_u = address,
			.address_v = address,
			.address_w = address
		};

		sampler->Initialize(args);

		return sampler;
	}

	TUnique<Texture> RenderCore::CreateTexture(UInt32 width, UInt32 height, ERHIResourceBindFlags flags, ERHIResourceFormat format, ERHIResourceDimension dimension, UInt32 array_size)
	{
		auto texture = std::make_unique<Texture>();

		Texture::Args args = {
			.rhi = m_rhi.get(),
			.resource_heap = m_resource_heap.get(),
			.rtv_heap = m_rtv_heap.get(),
			.dsv_heap = m_dsv_heap.get(),
			.width = width,
			.height = height,
			.depth = array_size,
			.format = format,
			.dimension = dimension,
			.bind_flags = flags,
		};

		texture->Initialize(args);
		return texture;
	}

	void RenderCore::UploadTexture(Texture* texture, void* pixels)
	{
		m_gpu_data_transfer->AddTexture(texture, pixels);
	}

	TUnique<Shader> RenderCore::CreateShader(const std::wstring& file_path, ERHIShaderType shader_type)
	{
		auto shader = std::make_unique<Shader>();

		Shader::Args desc = {};
		desc.rhi = m_rhi.get();
		desc.file_path = file_path.c_str();
		desc.type = shader_type;
		shader->Initialize(desc);

		return shader;
	}

	TUnique<PipelineLayout> RenderCore::CreatePipelineLayout(std::function<void(PipelineLayout&)> config_func)
	{
		auto pipeline_layout = std::make_unique<PipelineLayout>();

		if (config_func)
			config_func(*pipeline_layout);

		pipeline_layout->Build(m_rhi.get());

		return pipeline_layout;
	}

	TUnique<GraphicsPipeline> RenderCore::CreateGraphicsPipeline(GraphicsPipelineArgs args)
	{
		auto pipeline = std::make_unique<GraphicsPipeline>();
		args.rhi = m_rhi.get();

		pipeline->Initialize(args);

		return pipeline;
	}

	TUnique<ComputePipeline> RenderCore::CreateComputePipeline(ComputePipelineArgs args)
	{
		auto pipeline = std::make_unique<ComputePipeline>();
		args.rhi = m_rhi.get();

		pipeline->Initialize(args);

		return pipeline;
	}

	UINT32 RenderCore::GetCurrentBackBufferIndex() const
	{
		return m_swap_chain->GetCurrentBackBufferIndex();
	}

	Texture* RenderCore::GetCurrentRenderTarget() const
	{
		auto current_index = m_swap_chain->GetCurrentBackBufferIndex();
		return m_back_buffer[current_index].get();
	}

	Texture* RenderCore::GetDepthTexture() const
	{
		return m_depth_texture.get();
	}

	bool RenderCore::PostInitialize()
	{
		if (m_gpu_data_transfer->HasUpload())
			m_gpu_data_transfer->Upload();
		return true;
	}

	void RenderCore::BeginFrame()
	{
		// 今はリクエストをメインスレッドで監視してるけどマルチスレッドが追加されたら変える
		if (m_gpu_data_transfer->HasUpload())
		{
			UInt64 copy_fence_value = m_gpu_data_transfer->UploadAsync();

			m_command_queue->Wait(m_gpu_data_transfer->GetCopyTimeline(), copy_fence_value);

		}

		auto current_buffer_index = m_swap_chain->GetCurrentBackBufferIndex();

		// このフレームで使用する前に積んだコマンドが終了してないなら待つ
		m_timeline->WaitCPUCurrentFrame(current_buffer_index);

		m_resource_heap->Reclaim(current_buffer_index);
		m_sampler_heap->Reclaim(current_buffer_index);

		m_transient_allocator->Reset(current_buffer_index);

		m_command_list->Begin(current_buffer_index);

		m_command_list->Transition(GetCurrentRenderTarget(), ERHIResourceState::RenderTarget);
		m_command_list->Transition(GetDepthTexture(), ERHIResourceState::DepthWrite);

		m_command_list->SetRenderTarget(GetCurrentRenderTarget(), GetDepthTexture());
		m_command_list->ClearRenderTarget(GetCurrentRenderTarget(), Rect2D(0, 0, m_width, m_height), Color4::Gray);
		m_command_list->ClearDepthStencil(GetDepthTexture(), Rect2D(0, 0, m_width, m_height), 1.0f);
		m_command_list->SetViewport(Viewport(0, 0, (float)m_width, (float)m_height, 0.0f, 1.0f));

		m_command_list->SetScissorRect(0, 0, m_width, m_height);

	}

	void RenderCore::EndFrame()
	{
		m_command_list->Transition(GetDepthTexture(), ERHIResourceState::Common);
		m_command_list->Transition(GetCurrentRenderTarget(), ERHIResourceState::Present);

		m_command_list->End();

		m_command_queue->Execute(m_command_list.get());

		m_command_queue->Signal(m_timeline.get());

		m_swap_chain->Present();

	}

}
