#include "RenderCore.h"
#include "RenderHardwareInterface.h"

#include "SwapChain.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "Timeline.h"
#include "CommandLayout.h"

#include "DescriptorHeap.h"

#include "Buffer.h"

#include "Texture.h"

#include "SamplerState.h"

#include "Shader.h"

#include "VertexLayout.h"
#include "PipelineLayout.h"
#include "GraphicsPipeline.h"
#include "ComputePipeline.h"
#include "RayTracePipeline.h"

#include "GPUDataTransfer.h"
#include "ASBuilder.h"

#include "CommandListPool.h"

#include "ShaderCache.h"
#include "PipelineCache.h"

#include "TransientBufferAllocator.h"

#include "Core/Math/Vector.h"
#include "Core/Literal.h"
#include "Core/Math/HashUtil.h"


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
				.staging_buffer_size = 1024_MB,
			};
			m_gpu_data_transfer->Initialize(desc);
		}

		m_resource_heap = CreateDescriptorHeap(args.max_resource_num, ERHIDescriptorType::SRV_UAV_CBV);
		m_sampler_heap = CreateDescriptorHeap(args.max_sampler_num, ERHIDescriptorType::Sampler);

		m_preset_samplers.resize(+ESamplerPresetType::Count);

		// Point
		m_preset_samplers[+ESamplerPresetType::PointClamp] = CreateSamplerState(ERHIFilter::Point, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::PointClampClampWrap] = CreateSamplerState(ERHIFilter::Point, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::PointClampWrapClamp] = CreateSamplerState(ERHIFilter::Point, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::PointClampWrapWrap] = CreateSamplerState(ERHIFilter::Point, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::PointWrapClampClamp] = CreateSamplerState(ERHIFilter::Point, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::PointWrapClampWrap] = CreateSamplerState(ERHIFilter::Point, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::PointWrapWrapClamp] = CreateSamplerState(ERHIFilter::Point, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::PointWrap] = CreateSamplerState(ERHIFilter::Point, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap);

		// Linear
		m_preset_samplers[+ESamplerPresetType::LinearClamp] = CreateSamplerState(ERHIFilter::Linear, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::LinearClampClampWrap] = CreateSamplerState(ERHIFilter::Linear, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::LinearClampWrapClamp] = CreateSamplerState(ERHIFilter::Linear, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::LinearClampWrapWrap] = CreateSamplerState(ERHIFilter::Linear, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::LinearWrapClampClamp] = CreateSamplerState(ERHIFilter::Linear, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::LinearWrapClampWrap] = CreateSamplerState(ERHIFilter::Linear, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::LinearWrapWrapClamp] = CreateSamplerState(ERHIFilter::Linear, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::LinearWrap] = CreateSamplerState(ERHIFilter::Linear, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap);

		// Anisotropic
		m_preset_samplers[+ESamplerPresetType::AnisoClamp] = CreateSamplerState(ERHIFilter::Anisotropic, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::AnisoClampClampWrap] = CreateSamplerState(ERHIFilter::Anisotropic, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::AnisoClampWrapClamp] = CreateSamplerState(ERHIFilter::Anisotropic, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::AnisoClampWrapWrap] = CreateSamplerState(ERHIFilter::Anisotropic, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::AnisoWrapClampClamp] = CreateSamplerState(ERHIFilter::Anisotropic, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::AnisoWrapClampWrap] = CreateSamplerState(ERHIFilter::Anisotropic, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp, ERHIAddressMode::Wrap);
		m_preset_samplers[+ESamplerPresetType::AnisoWrapWrapClamp] = CreateSamplerState(ERHIFilter::Anisotropic, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap, ERHIAddressMode::Clamp);
		m_preset_samplers[+ESamplerPresetType::AnisoWrap] = CreateSamplerState(ERHIFilter::Anisotropic, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap, ERHIAddressMode::Wrap);

		// Shadow
		m_preset_samplers[+ESamplerPresetType::Shadow] = CreateSamplerState(ERHIFilter::ComparisonLinear, ERHIAddressMode::Border, ERHIAddressMode::Border, ERHIAddressMode::Border);
		
		
		m_as_builder = std::make_unique<ASBuilder>();
		{
			ASBuilder::Args desc = {
				.render_core = this,
				.staging_buffer_size = 128_MB,
			};
			m_as_builder->Initialize(desc);
		}

		m_direct_pool = std::make_unique<CommandListPool>();
		{
			CommandListPool::Args desc = {
				.render_core = this,
				.type = ERHICommandType::Direct,
			};
			m_direct_pool->Initialize(desc);
		}

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

		m_shader_cache = std::make_unique<ShaderCache>();
		{
			ShaderCache::Args desc = {
			};
			m_shader_cache->Initialize(desc);
		}

		m_pipeline_cache = std::make_unique<PipelineCache>();
		{
			PipelineCache::Args desc = {
			};
			m_pipeline_cache->Initialize(desc);
		}


		m_width = args.width;
		m_height = args.height;		
		return true;
	}

	void RenderCore::Finalize()
	{
		if (m_timeline)
		{
			m_timeline->WaitCPULatest();
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
		case EBufferPresetType::RTAccelStructure:
			desc.access_type = ERHIResourceAccessType::Static;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;
			desc.bind_flags |= ERHIResourceBindFlags::UnorderedAccess; // UAV追加
			desc.bind_flags |= ERHIResourceBindFlags::RayTracingAS; // レイトレ追加
			desc.size = Alignment(desc.size, 256);
			desc.stride = 1;
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

	ASBuildSize RenderCore::PreBuildBLAS(const BLASRequest& request)
	{
		RHIBuildBLASDesc build_desc = {};

		build_desc.vertex_count = request.vertex_count;

		for (auto& geo : request.geometries)
		{
			RHIBottomLevelGeometryDesc geometry =
			{
				.index_count = geo.index_count,
				.aabb_count = geo.procedural_count,
				.is_opaque = geo.is_opaque,
			};

			build_desc.geometries.push_back(geometry);
		}

		build_desc.allow_update = request.allow_update;

		RHIASMemoryInfo info = {};

		m_rhi->RequestBottomLevelASSize(build_desc, &info);

		ASBuildSize size = {};
		size.result = info.size;
		size.scratch = info.scratch_size;
		size.update = info.update_size;

		return size;
	}

	ASBuildSize RenderCore::PreBuildTLAS(const TLASRequest& request)
	{
		RHIBuildTLASDesc build_desc = {};

		build_desc.instance_count = request.instance_count;

		RHIASMemoryInfo info = {};

		m_rhi->RequestTopLevelASSize(build_desc, &info);

		ASBuildSize size = {};
		size.result = info.size;
		size.scratch = info.scratch_size;
		size.update = info.update_size;

		return size;
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

	TUnique<SamplerState> RenderCore::CreateSamplerState(ERHIFilter filter, ERHIAddressMode address_u, ERHIAddressMode address_v, ERHIAddressMode address_w)
	{
		auto sampler = std::make_unique<SamplerState>();

		SamplerState::Args args = {
			.rhi = m_rhi.get(),
			.sampler_heap = m_sampler_heap.get(),
			.filter = filter,
			.address_u = address_u,
			.address_v = address_v,
			.address_w = address_w
		};

		sampler->Initialize(args);

		return sampler;
	}

	TUnique<Texture> RenderCore::CreateTexture(UInt32 width, UInt32 height, UInt32 depth, ERHIResourceFormat format, ETexturePresetType type)
	{
		auto texture = std::make_unique<Texture>();

		Texture::Args desc = {
			.rhi = m_rhi.get(),
			.resource_heap = m_resource_heap.get(),
			.rtv_heap = m_rtv_heap.get(),
			.dsv_heap = m_dsv_heap.get(),
			.width = width,
			.height = height,
			.depth = depth,
			.format = format,
		};

		switch (type)
		{
		case ETexturePresetType::ROTexture2D:
			desc.dimension = ERHIResourceDimension::Tex2D;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;
			break;
		case ETexturePresetType::RWTexture2D:
			desc.dimension = ERHIResourceDimension::Tex2D;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;
			desc.bind_flags |= ERHIResourceBindFlags::UnorderedAccess;
			break;
		case ETexturePresetType::RWTexture3D:
			desc.dimension = ERHIResourceDimension::Tex3D;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;
			desc.bind_flags |= ERHIResourceBindFlags::UnorderedAccess;
			break;
		case ETexturePresetType::RenderTarget2D:
			desc.dimension = ERHIResourceDimension::Tex2D;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;
			desc.bind_flags |= ERHIResourceBindFlags::RenderTarget;
			break;
		case ETexturePresetType::DepthStencil2D:
			desc.dimension = ERHIResourceDimension::Tex2D;
			desc.bind_flags |= ERHIResourceBindFlags::ShaderResource;
			desc.bind_flags |= ERHIResourceBindFlags::DepthStencil;
			break;
		default:
			break;
		}

		texture->Initialize(desc);
		return texture;
	}

	TUnique<Texture> RenderCore::CreateTexture(UInt32 width, UInt32 height, ERHIResourceBindFlags flags, ERHIResourceFormat format, ERHIResourceDimension dimension, UInt32 array_size, UInt32 mip_level)
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
			.mip_level = mip_level,
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

	Shader* RenderCore::CreateShader(StringView file_path, ERHIShaderType shader_type, StringView entry_point)
	{
		if (Shader* cached_shader = m_shader_cache->Find(file_path, shader_type, entry_point))
		{
			return cached_shader;
		}

		auto shader = std::make_unique<Shader>();

		Shader::Args desc = {};
		desc.rhi = m_rhi.get();
		desc.file_path = file_path.c_str();
		desc.entry_points.push_back(entry_point.c_str());
		desc.type = shader_type;
		shader->Initialize(desc);

		return m_shader_cache->Add(file_path, std::move(shader));
	}

	Shader* RenderCore::CreateShaderLibrary(StringView file_path, std::span<const StringView> entry_points)
	{
		if (Shader* cached_shader = m_shader_cache->Find(file_path, entry_points))
		{
			return cached_shader;
		}

		auto shader = std::make_unique<Shader>();

		Shader::Args desc = {};
		desc.rhi = m_rhi.get();
		desc.file_path = file_path.c_str();
		desc.entry_points.assign(entry_points.begin(), entry_points.end());
		desc.type = ERHIShaderType::Library;
		shader->Initialize(desc);

		return m_shader_cache->Add(file_path, std::move(shader));
	}

	PipelineLayout* RenderCore::CreatePipelineLayout(std::function<void(PipelineLayout&)> config_func)
	{
		auto pipeline_layout = std::make_unique<PipelineLayout>();

		if (config_func)
			config_func(*pipeline_layout);

		UInt64 handle_id = pipeline_layout->GetHash();


		if (PipelineLayout* cached_pipeline_layout = m_pipeline_cache->FindLayout(handle_id))
		{
			return cached_pipeline_layout;
		}


		pipeline_layout->Build(m_rhi.get());

		return m_pipeline_cache->Add(handle_id, std::move(pipeline_layout));
	}

	GraphicsPipeline* RenderCore::CreateGraphicsPipeline(const GraphicsPipelineArgs& args)
	{
		UInt64 handle_id = 0;

		HashCombine(handle_id, args.pipeline_layout);
		HashCombine(handle_id, args.vertex_layout.GetHash());

		HashCombine(handle_id, args.vertex_shader);
		HashCombine(handle_id, args.geometry_shader);
		HashCombine(handle_id, args.hull_shader);
		HashCombine(handle_id, args.domain_shader);
		HashCombine(handle_id, args.task_shader);
		HashCombine(handle_id, args.mesh_shader);
		HashCombine(handle_id, args.pixel_shader);

		HashCombine(handle_id, args.render_target_count);

		HashCombine(handle_id, args.alpha_to_coverage_enable);
		HashCombine(handle_id, args.independent_blend_enable);

		for (UInt32 i = 0; i < args.render_target_count; ++i)
		{
			HashCombine(handle_id, static_cast<UInt32>(args.render_targets[i]));

			const auto& blend = args.blend_states[i];
			HashCombine(handle_id, blend.blend_enable);
			HashCombine(handle_id, static_cast<UInt32>(blend.src_blend));
			HashCombine(handle_id, static_cast<UInt32>(blend.dest_blend));
			HashCombine(handle_id, static_cast<UInt32>(blend.blend_op));
			HashCombine(handle_id, static_cast<UInt32>(blend.src_blend_alpha));
			HashCombine(handle_id, static_cast<UInt32>(blend.dest_blend_alpha));
			HashCombine(handle_id, static_cast<UInt32>(blend.blend_op_alpha));
			HashCombine(handle_id, blend.write_mask);
		}

		HashCombine(handle_id, static_cast<UInt32>(args.cull_mode));
		HashCombine(handle_id, static_cast<UInt32>(args.primitive_topology));
		HashCombine(handle_id, static_cast<UInt32>(args.depth_format));
		HashCombine(handle_id, args.depth_enable);

		HashCombine(handle_id, args.depth_write_enable);
		HashCombine(handle_id, static_cast<UInt32>(args.depth_compare_op));


		HashCombine(handle_id, static_cast<UInt32>(args.fill_mode));


		if (GraphicsPipeline* cached_pipeline = m_pipeline_cache->FindGraphics(handle_id))
		{
			return cached_pipeline;
		}

		auto pipeline = std::make_unique<GraphicsPipeline>();

		GraphicsPipelineArgs desc = args;
		desc.rhi = m_rhi.get();

		pipeline->Initialize(desc);

		return m_pipeline_cache->Add(handle_id, std::move(pipeline));
	}

	ComputePipeline* RenderCore::CreateComputePipeline(const ComputePipelineArgs& args)
	{
		UInt64 handle_id = 0;
		HashCombine(handle_id, args.pipeline_layout);
		HashCombine(handle_id, args.compute_shader);

		if (ComputePipeline* cached_pipeline = m_pipeline_cache->FindCompute(handle_id))
		{
			return cached_pipeline;
		}

		auto pipeline = std::make_unique<ComputePipeline>();

		ComputePipelineArgs desc = args;
		desc.rhi = m_rhi.get();

		pipeline->Initialize(desc);

		return m_pipeline_cache->Add(handle_id, std::move(pipeline));
	}

	RayTracePipeline* RenderCore::CreateRayTracePipeline(const RayTracePipelineArgs& args)
	{
		UInt64 handle_id = 0;

		HashCombine(handle_id, args.pipeline_layout);
		HashCombine(handle_id, args.max_payload_size);
		HashCombine(handle_id, args.max_attribute_size);
		HashCombine(handle_id, args.max_recursion_depth);

		HashCombine(handle_id, static_cast<UInt64>(args.libraries.size()));
		for (size_t i = 0; i < args.libraries.size(); ++i)
		{
			HashCombine(handle_id, args.libraries[i]);
		}

		HashCombine(handle_id, args.ray_gen_entry_name);

		HashCombine(handle_id, static_cast<UInt64>(args.miss_entry_names.size()));
		for (size_t i = 0; i < args.miss_entry_names.size(); ++i)
		{
			HashCombine(handle_id, args.miss_entry_names[i]);
		}

		HashCombine(handle_id, static_cast<UInt64>(args.hit_groups.size()));
		for (size_t i = 0; i < args.hit_groups.size(); ++i)
		{
			const auto& hg = args.hit_groups[i];
			HashCombine(handle_id, hg.name);
			HashCombine(handle_id, static_cast<UInt32>(hg.type));
			HashCombine(handle_id, hg.any_hit_entry_name);
			HashCombine(handle_id, hg.closest_hit_entry_name);
			HashCombine(handle_id, hg.intersection_entry_name);
		}

		if (RayTracePipeline* cached_pipeline = m_pipeline_cache->FindRayTrace(handle_id))
		{
			return cached_pipeline;
		}

		auto pipeline = std::make_unique<RayTracePipeline>();

		RayTracePipelineArgs desc = args;
		desc.rhi = m_rhi.get();

		pipeline->Initialize(desc);

		return m_pipeline_cache->Add(handle_id, std::move(pipeline));
	}

	CommandLayout* RenderCore::CreateCommandLayout(const CommandLayoutArgs& args)
	{
		UInt64 handle_id = 0;

		HashCombine(handle_id, args.pipeline_layout);

		HashCombine(handle_id, static_cast<UInt64>(args.arguments.size()));
		for (size_t i = 0; i < args.arguments.size(); ++i)
		{
			const auto& arg = args.arguments[i];

			HashCombine(handle_id, static_cast<UInt32>(arg.type));
			HashCombine(handle_id, arg.root_parameter_index);
			HashCombine(handle_id, arg.dest_runtime_constant_offset);
			HashCombine(handle_id, arg.num_32bit_values_to_set);
			HashCombine(handle_id, arg.slot_index);
		}


		if (CommandLayout* cached_pipeline_layout = m_pipeline_cache->FindCommandLayout(handle_id))
		{
			return cached_pipeline_layout;
		}

		auto command_layout = std::make_unique<CommandLayout>();

		CommandLayoutArgs desc = args;
		desc.rhi = m_rhi.get();

		command_layout->Initialize(desc);

		return m_pipeline_cache->Add(handle_id, std::move(command_layout));

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

	CommandList* RenderCore::AcquireCommandList() const
	{
		auto list = m_direct_pool->AcquireCommandList();

		auto current_buffer_index = m_swap_chain->GetCurrentBackBufferIndex();

		list->Begin(current_buffer_index);

		list->SetDescriptorHeap(m_resource_heap.get(), m_sampler_heap.get());

		return list;
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

		m_direct_pool->Reset();

		m_as_builder->Reset(current_buffer_index);
	}

	void RenderCore::EndFrame()
	{
		auto active_lists = m_direct_pool->GetActiveCommandLists();

		for (CommandList* list : active_lists)
		{
			list->End();
		}

		m_command_queue->Execute(active_lists);
		m_command_queue->Signal(m_timeline.get());

		m_swap_chain->Present();

	}

}
