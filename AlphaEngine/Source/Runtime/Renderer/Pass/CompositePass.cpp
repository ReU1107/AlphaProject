#include "CompositePass.h"
#include "RenderCore/Module.h"
#include "../RenderGraph.h"
#include "../RenderGraphBuilder.h"

#include "Core/Math/MathUtil.h"

namespace Alpha::Renderer
{
	struct CompositePassConstants
	{
		UInt32 result_index;
		UInt32 volumetric;
		UInt32 transmittance;
		UInt32 scene_base;
	};

	void AddCompositePass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph)
	{
		auto shader = RenderCore::Get().CreateShader(L"Asset/Shader/CompositeCS.hlsl", ERHIShaderType::Compute);

		ComputePipeline* pipeline = RenderCore::Get().CreateComputePipeline({
			.pipeline_layout = layout,
			.compute_shader = shader
			}
		);

		auto scene_color = graph->CreateTexture(L"SceneColor",
			{
				1920, 1080, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::RenderTarget | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);

		auto radiance = graph->CreateTexture(L"RadianceVolume",
			{
				480, 270, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);
		auto transmittance = graph->CreateTexture(L"Transmittance",
			{
				480, 270, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);

		auto lit_scene_color = graph->CreateTexture(L"LitSceneColor",
			{
				1920, 1080, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);

		graph->AddPass(L"Composite",
			[=](RenderGraphBuilder& builder)
			{
				builder.WriteTexture(lit_scene_color, ERHIResourceState::UnorderedAccess);
				builder.ReadTexture(radiance, ERHIResourceState::AllShaderResource);
				builder.ReadTexture(transmittance, ERHIResourceState::AllShaderResource);
				builder.ReadTexture(scene_color, ERHIResourceState::AllShaderResource);

				builder.UsePipeline(pipeline, ERHIPipelineType::Compute);
			},
			[=](RenderGraphContext& ctx)
			{
				Texture* result_tex = ctx.graph->GetTexture(lit_scene_color);
				Texture* radiance_tex = ctx.graph->GetTexture(radiance);
				Texture* transmittance_tex = ctx.graph->GetTexture(transmittance);
				Texture* scene_color_tex = ctx.graph->GetTexture(scene_color);

				CompositePassConstants constants = {};
				constants.result_index = result_tex->GetUAVHandle().index;
				constants.volumetric = radiance_tex->GetSRVHandle().index;
				constants.transmittance = transmittance_tex->GetSRVHandle().index;
				constants.scene_base = scene_color_tex->GetSRVHandle().index;

				auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);
				ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

				UInt32 group_x = RoundUp(1920, 8);
				UInt32 group_y = RoundUp(1080, 8);
				ctx.command_list->Dispatch(group_x, group_y, 1);
			}
		);
	}
}