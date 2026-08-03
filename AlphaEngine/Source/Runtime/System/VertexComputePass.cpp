#include "VertexComputePass.h"
#include "RenderCore/Module.h"
#include "Renderer/Module.h"

namespace Alpha
{
	VertexComputePass::~VertexComputePass() = default;
	bool VertexComputePass::Initialize(const Args& args)
	{
		m_cs = RenderCore::Get().CreateShader(L"Asset/AlignVertices.hlsl", ERHIShaderType::Compute);

		m_align_vertex_pipeline = RenderCore::Get().CreateComputePipeline(
			{
				.pipeline_layout = args.layout,
				.compute_shader = m_cs.get(),
			}
		);

		return true;
	}

	void VertexComputePass::Finalize()
	{
	}

	void VertexComputePass::Execute(RenderContext& ctx, Buffer* dynamic_vertex_buffer, UInt64 vertex_count)
	{
		// ComputeShaderでのスキニングを行うにしろしないにしろこの頂点バッファに書き込む
		// 粒度は頂点単位
		ctx.command_list->SetComputePipeline(m_align_vertex_pipeline.get());

		ctx.command_list->Transition(dynamic_vertex_buffer, ERHIResourceState::UnorderedAccess);

		uint32_t dispatch_x = (vertex_count + 63) / 64;

		ctx.command_list->Dispatch(dispatch_x, 1, 1);

		ctx.command_list->UAVBarrier(dynamic_vertex_buffer);

		ctx.command_list->Transition(dynamic_vertex_buffer, ERHIResourceState::NonPixelShaderResource);
	}
}