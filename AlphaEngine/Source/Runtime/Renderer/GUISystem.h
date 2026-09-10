#pragma once
#include "Core/Common.h"
#include "Core/Subsystem.h"

namespace Alpha
{
	class RenderCore;
	class TransientBufferAllocator;
	class Texture;
	class Shader;
	class PipelineLayout;
	class GraphicsPipeline;
}

namespace Alpha::Renderer
{
	struct RenderContext;
	class RenderGraph;

	class GUISystem : public Subsystem<GUISystem>
	{
	private:
		TUnique<TransientBufferAllocator> m_vertex_buffer{};
		TUnique<TransientBufferAllocator> m_index_buffer{};
		TUnique<Texture> m_font_texture{};

	public:
		GUISystem();
		~GUISystem();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			RenderGraph* graph{ nullptr };
			uint32_t max_vertex_num{ 1024 };

		};
		bool Initialize(const Args& args);
		void Finalize();

		// フレームの開始 (入力更新などを内部で隠蔽)
		void BeginFrame();

		// フレームの終了 (命令の構築を隠蔽)
		void EndFrame();


	};

}