#include "VertexComputePass.h"
#include "RenderCore/Module.h"
#include "Renderer/Module.h"
#include "Core/Math/MathUtil.h"

namespace Alpha::Renderer
{
	VertexComputePass::~VertexComputePass() = default;
	bool VertexComputePass::Initialize(const Args& args)
	{
		m_cs = RenderCore::Get().CreateShader(L"Asset/Shader/AlignVerticesCS.hlsl", ERHIShaderType::Compute);

		m_align_vertex_pipeline = RenderCore::Get().CreateComputePipeline(
			{
				.pipeline_layout = args.layout,
				.compute_shader = m_cs,
			}
		);

		return true;
	}

	void VertexComputePass::Finalize()
	{
	}

	void VertexComputePass::Execute(RenderContext& ctx, Buffer* dynamic_vertex_buffer, UInt64 vertex_count)
	{
		if (vertex_count == 0)
			return;

		// スキニングや頂点変形を行う
		// 粒度は頂点単位
		ctx.command_list->SetComputePipeline(m_align_vertex_pipeline);

		ctx.command_list->Transition(dynamic_vertex_buffer, ERHIResourceState::UnorderedAccess);

		UInt32 dispatch_x = RoundUp(vertex_count, 64);

		ctx.command_list->Dispatch(dispatch_x, 1, 1);

		ctx.command_list->UAVBarrier(dynamic_vertex_buffer);

		ctx.command_list->Transition(dynamic_vertex_buffer, ERHIResourceState::NonPixelShaderResource);
	}
}