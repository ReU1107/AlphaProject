#include "ParticleRender.h"
#include "RenderCore/Module.h"

namespace Alpha
{
	ParticleRender::ParticleRender() = default;
	ParticleRender::~ParticleRender() = default;

	bool ParticleRender::Initialize(const Args& args)
	{

		m_command_layout = args.render_core->CreateCommandLayout(
			{
				.arguments = {
					{
						.type = ERHIIndirectArgType::DispatchMesh,
					},
				},
			}
			);

		m_command_buffer = args.render_core->CreateBuffer(sizeof(DispatchMeshArgs), sizeof(DispatchMeshArgs), EBufferPresetType::RWStructured);

		m_prerender_cs = args.render_core->CreateShader(L"Asset/ParticlePrerenderCS.hlsl", ERHIShaderType::Compute);

		//m_layout = args.render_core->CreatePipelineLayout(
		//	[](PipelineLayout& layout)
		//	{
		//	layout.AddRootCBV(1, ERHIShaderFlags::All);
		//	layout.AddRootCBV(1, ERHIShaderFlags::All);
		//	layout.SetFlags(ERHIPipelineLayoutFlags::Bindless);
		//	}
		//);

		//m_prerender_pipeline = args.render_core->CreateComputePipeline(
		//	{
		//		.pipeline_layout = m_layout.get(),
		//		.compute_shader = m_prerender_cs.get(),
		//	}
		//);

		return true;
	}

	void ParticleRender::Finalize()
	{
	}

	void ParticleRender::Render(RenderContext& ctx)
	{
		ctx.command_list->Transition(m_command_buffer.get(), ERHIResourceState::UnorderedAccess);

		ctx.command_list->SetComputePipeline(m_prerender_pipeline.get());
		ctx.command_list->Dispatch(1, 1, 1);

		ctx.command_list->Transition(m_command_buffer.get(), ERHIResourceState::IndirectArgument);



	}
}