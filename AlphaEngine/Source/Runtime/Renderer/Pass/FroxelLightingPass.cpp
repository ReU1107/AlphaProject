#include "FroxelLightingPass.h"
#include "RenderCore/Module.h"
#include "../RenderGraph.h"
#include "../RenderGraphBuilder.h"
#include "Core/Math/MathUtil.h"

namespace Alpha::Renderer
{
	static UInt32 ResolutionX = 160;
	static UInt32 ResolutionY = 90;
	static UInt32 ResolutionZ = 64;

	struct FroxelLightingPassConstants
	{
		UInt32 result_index;
	};

	void AddFroxelLightingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph)
	{
		auto shader = RenderCore::Get().CreateShader(L"Asset/Shader/FroxelLightingCS.hlsl", ERHIShaderType::Compute);

		ComputePipeline* pipeline = RenderCore::Get().CreateComputePipeline({
			.pipeline_layout = layout,
			.compute_shader = shader
			}
		);

		auto froxel_radiance = graph->CreateTexture(L"FroxelRadiance",
			{
				ResolutionX, ResolutionY, ResolutionZ,
				ERHIResourceDimension::Tex3D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);

		graph->AddPass(L"FroxelLighting",
			[=](RenderGraphBuilder& builder)
			{
				builder.WriteTexture(froxel_radiance, ERHIResourceState::UnorderedAccess);
				builder.UsePipeline(pipeline, ERHIPipelineType::Compute);
			},
			[=](RenderGraphContext& ctx)
			{
				Texture* result_tex = ctx.graph->GetTexture(froxel_radiance);

				FroxelLightingPassConstants constants = {};
				constants.result_index = result_tex->GetUAVHandle().index;

				auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);
				ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

				UInt32 group_x = RoundUp(ResolutionX, 8);
				UInt32 group_y = RoundUp(ResolutionY, 8);
				UInt32 group_z = RoundUp(ResolutionZ, 4);
				ctx.command_list->Dispatch(group_x, group_y, group_z);
			}
		);
	}

}