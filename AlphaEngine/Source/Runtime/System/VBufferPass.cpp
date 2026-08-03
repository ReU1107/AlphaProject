#include "VBufferPass.h"
#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include <imgui/imgui.h>


namespace Alpha
{
	VBufferPass::~VBufferPass()
	{
	}

	bool VBufferPass::Initialize(const Args& args)
	{

		m_as = RenderCore::Get().CreateShader(L"Asset/TaskShader.hlsl", ERHIShaderType::Amplification);
		//m_ms = RenderCore::Get().CreateShader(L"Asset/TestMeshShader.hlsl", ERHIShaderType::Mesh);
		m_ms = RenderCore::Get().CreateShader(L"Asset/VBufferMS.hlsl", ERHIShaderType::Mesh);
		//m_ps = RenderCore::Get().CreateShader(L"Asset/PixelShader.hlsl", ERHIShaderType::Pixel);
		m_ps = RenderCore::Get().CreateShader(L"Asset/VBufferPS.hlsl", ERHIShaderType::Pixel);

		m_visibility = RenderCore::Get().CreateTexture(
			1920, 1080,
			ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::RenderTarget,
			ERHIResourceFormat::RGUInt
		);

		m_depth = RenderCore::Get().CreateTexture(
			1920, 1080,
			ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::DepthStencil,
			ERHIResourceFormat::Depth32
		);

		m_pipeline = RenderCore::Get().CreateGraphicsPipeline(
			{
				.pipeline_layout = args.layout,
				.task_shader = m_as.get(),
				.mesh_shader = m_ms.get(),
				.pixel_shader = m_ps.get(),
				.render_target_count = 1,
				.render_targets = {
					m_visibility->GetFormat(),
				},
				.blend_states = {
					DisabledBlend(),
				},
				.cull_mode = ERHICullMode::None,
				.depth_format = m_depth->GetFormat(),
				.depth_enable = true,
			}
		);

		return true;
	}

	void VBufferPass::Finalize()
	{
	}

	void VBufferPass::Execute(RenderContext& ctx)
	{
		Texture* render_targets[] = {
			m_visibility.get(),
		};

		ctx.command_list->SetRenderTargets(render_targets, 1, m_depth.get());

		ctx.command_list->Transition(render_targets, 1, ERHIResourceState::RenderTarget);
		ctx.command_list->Transition(m_depth.get(), ERHIResourceState::DepthWrite);

		ctx.command_list->ClearRenderTargets(render_targets, 1, Rect2D(0, 0, 1920, 1080), Color4::Gray);
		ctx.command_list->ClearDepthStencil(m_depth.get(), Rect2D(0, 0, 1920, 1080), 0.0f);

		ctx.command_list->SetGraphicsPipeline(m_pipeline.get());


		ctx.command_list->DispatchMesh(ctx.instance_count, 1, 1);

		// Debug限定でAllにしてる　本来はNonPixel
		ctx.command_list->Transition(render_targets, 1, ERHIResourceState::AllShaderResource);
		ctx.command_list->Transition(m_depth.get(), ERHIResourceState::AllShaderResource);

		ImGui::Begin("VBuffer Pass"); 

		ImGui::Image(m_visibility.get(), ImVec2(960, 540));

		ImGui::End();

	}
}