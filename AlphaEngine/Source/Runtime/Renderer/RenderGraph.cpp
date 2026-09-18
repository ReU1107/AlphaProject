#include "RenderGraph.h"
#include "RenderCore/Module.h"

#include "Core/Math/HashUtil.h"

#include "RenderGraphBuilder.h"

namespace
{
	using namespace Alpha;
	using namespace Alpha::Renderer;

	inline UInt64 GetHash(const RGTextureDesc& desc)
	{
		UInt64 seed = 0;

		HashCombine(seed, desc.width);
		HashCombine(seed, desc.height);
		HashCombine(seed, desc.format);
		HashCombine(seed, desc.flags);

		return seed;
	}
}

namespace Alpha::Renderer
{
	RenderGraph::~RenderGraph() = default;

	bool RenderGraph::Initialize(const Args& args)
	{
		m_render_core = args.render_core;
		m_texture_palette.Initialize(100);
		m_buffer_palette.Initialize(100);
		return true;
	}

	RGTextureHandle RenderGraph::CreateTexture(StringView name, const RGTextureDesc& desc)
	{

		// 一旦名前でハッシュ値にしとく
		UInt64 handle_id = 0;

		HashCombine(handle_id, name);


		UInt32 palette_index = 0;

		if (m_texture_palette.TryGetIndex(handle_id, &palette_index))
		{
			return RGTextureHandle(palette_index);
		}

		RGTextureResource& resource = m_texture_palette.Allocate(handle_id, palette_index);
		
		resource.name = name;
		resource.desc = desc;
		resource.imported = false;

		auto texture = m_render_core->CreateTexture(desc.width, desc.height, desc.flags, desc.format, desc.dimension, desc.depth);

		texture->SetName(name);

		resource.target = texture.get();

		m_textures.push_back(std::move(texture));

		return RGTextureHandle(palette_index);
	}

	RGTextureHandle RenderGraph::ImportTexture(StringView name, Texture* texture)
	{
		UInt64 handle_id = 0;
		HashCombine(handle_id, name);

		UInt32 palette_index = 0;

		if (m_texture_palette.TryGetIndex(handle_id, &palette_index))
		{
			return RGTextureHandle(palette_index);
		}

		RGTextureResource& resource = m_texture_palette.Allocate(handle_id, palette_index);
		resource.name = name;
		resource.imported = true;
		resource.target = texture;

		return RGTextureHandle(palette_index);
	}

	RGBufferHandle RenderGraph::CreateBuffer(StringView name, const RGBufferDesc& desc)
	{
		UInt64 handle_id = 0;
		HashCombine(handle_id, name);

		UInt32 palette_index = 0;

		if (m_buffer_palette.TryGetIndex(handle_id, &palette_index))
		{
			const RGBufferResource& resource = m_buffer_palette.GetAt(palette_index);

			return RGBufferHandle(palette_index);
		}

		RGBufferResource& resource = m_buffer_palette.Allocate(handle_id, palette_index);

		resource.name = name;
		resource.desc = desc;
		resource.imported = false;

		auto buffer = m_render_core->CreateBuffer(
			desc.stride,
			desc.size,
			desc.type);

		buffer->SetName(name);

		resource.target = buffer.get();
		m_buffers.push_back(std::move(buffer));

		return RGBufferHandle(palette_index);
	}

	RGBufferHandle RenderGraph::ImportBuffer(StringView name, Buffer* buffer)
	{
		UInt64 handle_id = 0;
		HashCombine(handle_id, name);

		UInt32 palette_index = 0;

		if (m_buffer_palette.TryGetIndex(handle_id, &palette_index))
		{
			return RGBufferHandle(palette_index);
		}

		RGBufferResource& resource = m_buffer_palette.Allocate(handle_id, palette_index);
		resource.name = name;
		resource.imported = true;
		resource.target = buffer;

		return RGBufferHandle(palette_index);
	}

	void RenderGraph::UpdateImportedTexture(RGTextureHandle handle, Texture* texture)
	{
		// 本来はここでチェックとか行う必要があるけど

		m_texture_palette.GetAt(handle.index).target = texture;


	}

	void RenderGraph::UpdateImportedBuffer(RGBufferHandle handle, Buffer* buffer)
	{
		m_buffer_palette.GetAt(handle.index).target = buffer;
	}

	void RenderGraph::AddPass(StringView name, PassSetupFunc&& setup, PassExecuteFunc&& execute)
	{
		RenderGraphPass pass{};

		RenderGraphBuilder builder(this, &pass);

		setup(builder);

		pass.name = name;
		pass.execute = execute;

		m_passes.push_back(pass);
	}

	Texture* RenderGraph::GetTexture(RGTextureHandle handle) const
	{
		return m_texture_palette.GetAt(handle.index).target;
	}

	Texture* RenderGraph::FindTexture(StringView name) const
	{
		UInt64 handle_id = 0;

		HashCombine(handle_id, name);


		UInt32 palette_index = 0;

		if (m_texture_palette.TryGetIndex(handle_id, &palette_index))
		{
			return m_texture_palette.GetAt(palette_index).target;
		}

		return nullptr;
	}

	Buffer* RenderGraph::GetBuffer(RGBufferHandle handle) const
	{
		return m_buffer_palette.GetAt(handle.index).target;
	}

	void RenderGraph::Execute(RenderContext& ctx)
	{
		RenderGraphContext graph_ctx = {};
		graph_ctx.command_list = ctx.command_list;
		graph_ctx.instance_count = ctx.instance_count;
		graph_ctx.graph = this;
		graph_ctx.current_back_buffer = ctx.current_back_buffer;
		graph_ctx.current_frame_index = ctx.current_frame_index;


		for (auto& pass : m_passes)
		{
			for (auto& texture : pass.textures)
			{
				const RGTextureResource& resource = m_texture_palette.GetAt(texture.handle.index);

				ctx.command_list->Transition(resource.target, texture.state);
			}

			std::visit([&](auto&& val) -> void {
				using T = std::decay_t<decltype(val)>;

				if constexpr (std::is_same_v<T, std::monostate>) {
					return;
				}
				else {
					// 安全のためヌルポインタチェック
					if (!val) return;
					if constexpr (std::is_same_v<T, GraphicsPipeline*>)
						ctx.command_list->SetGraphicsPipeline(val);
					else if constexpr (std::is_same_v<T, ComputePipeline*>)
						ctx.command_list->SetComputePipeline(val);
					else if constexpr (std::is_same_v<T, RayTracePipeline*>)
						ctx.command_list->SetRayTracePipeline(val);
				}
			}, pass.pipeline.handle);



			pass.execute(graph_ctx);

		}

	}
}