#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;
	class ComputePipeline;
	class PipelineLayout;
	class Shader;
	class Buffer;
}

namespace Alpha::Renderer
{
	struct RenderContext;

	class VertexComputePass
	{
	private:
		ComputePipeline* m_align_vertex_pipeline{ nullptr };
		Shader* m_cs{ nullptr };


	public:
		~VertexComputePass();
		struct Args
		{
			RenderCore* render_core{ nullptr };
			PipelineLayout* layout{ nullptr };
		};
		bool Initialize(const Args& args);
		void Finalize();

		void Execute(RenderContext& ctx, Buffer* dynamic_vertex_buffer, UInt64 vertex_count);
	};

}