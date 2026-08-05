#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;
	class PipelineLayout;
	class ComputePipeline;
	class Shader;
	class Buffer;
	struct RenderContext;

	class VertexComputePass
	{
	private:
		TUnique<ComputePipeline> m_align_vertex_pipeline{ nullptr };
		TUnique<Shader> m_cs{ nullptr };


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