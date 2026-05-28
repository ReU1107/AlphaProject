#pragma once
#include "Core/Subsystem.h"
#include "Core/Types.h"

namespace Alpha
{
	struct RenderContext;

	class RenderCore;
	class RingBuffer;
	class Texture;
	class Shader;
	class PipelineLayout;
	class GraphicsPipeline;

	class GUISystem : public Subsystem<GUISystem>
	{
	private:
		TUnique<RingBuffer> m_vertex_buffer{};
		TUnique<RingBuffer> m_index_buffer{};
		TUnique<Texture> m_font_texture{};

		TUnique<Shader> m_vs{};
		TUnique<Shader> m_ps{};
		TUnique<PipelineLayout> m_pipeline_layout{};
		TUnique<GraphicsPipeline> m_pipeline{};

	public:
		GUISystem();
		~GUISystem();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			uint32_t max_vertex_num{ 1024 };

		};
		bool Initialize(const Args& args);
		void Finalize();

		// フレームの開始 (入力更新などを内部で隠蔽)
		void BeginFrame();

		// フレームの終了 (命令の構築を隠蔽)
		void EndFrame();

		void Flush(RenderContext& ctx);


	};

}