#include "FroxelLightingPass.h"
#include "RenderCore/Module.h"
#include "../RenderGraph.h"
#include "../RenderGraphBuilder.h"
#include "../RenderSettings.h"
#include "Core/Math/MathUtil.h"

namespace Alpha::Renderer
{
	struct FroxelLightingPassConstants
	{
		uint cluster;
		uint light_cluster;
		uint light_indices;
		uint result_index;
	};

	FroxelLightingResources AddFroxelLightingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph, const FroxelLightingSettings& froxel_settings, const ClusterLightResources& cluster_resources)
	{
		auto shader = RenderCore::Get().CreateShader(L"Asset/Shader/FroxelLightingCS.hlsl", ERHIShaderType::Compute);

		ComputePipeline* pipeline = RenderCore::Get().CreateComputePipeline({
			.pipeline_layout = layout,
			.compute_shader = shader
			}
		);

		UInt32 dimension_x = froxel_settings.grid_dimensions.X();
		UInt32 dimension_y = froxel_settings.grid_dimensions.Y();
		UInt32 dimension_z = froxel_settings.grid_dimensions.Z();

		auto froxel_radiance = graph->CreateTexture(L"FroxelRadiance",
			{
				dimension_x, dimension_y, dimension_z,
				ERHIResourceDimension::Tex3D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);

		auto cluster = cluster_resources.cluster_texture;

		auto light_cluster = cluster_resources.cluster_buffer;

		auto light_indices = cluster_resources.light_indices;

		graph->AddPass(L"FroxelLighting",
			[=](RenderGraphBuilder& builder)
			{
				builder.WriteTexture(froxel_radiance, ERHIResourceState::UnorderedAccess);
				builder.ReadTexture(cluster, ERHIResourceState::AllShaderResource);
				builder.ReadBuffer(light_cluster, ERHIResourceState::AllShaderResource);
				builder.ReadBuffer(light_indices, ERHIResourceState::AllShaderResource);

				builder.UsePipeline(pipeline, ERHIPipelineType::Compute);
			},
			[=](RenderGraphContext& ctx)
			{
				auto* result_tex = ctx.graph->GetTexture(froxel_radiance);
				auto* cluster_tex = ctx.graph->GetTexture(cluster);
				auto* light_cluster_buffer = ctx.graph->GetBuffer(light_cluster);
				auto* light_indices_buffer = ctx.graph->GetBuffer(light_indices);

				FroxelLightingPassConstants constants = {};
				constants.result_index = result_tex->GetUAVHandle().index;
				constants.cluster = cluster_tex->GetSRVHandle().index;
				constants.light_cluster = light_cluster_buffer->GetSRVHandle().index;
				constants.light_indices = light_indices_buffer->GetSRVHandle().index;

				auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);
				ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

				UInt32 group_x = RoundUp(dimension_x, 8);
				UInt32 group_y = RoundUp(dimension_y, 8);
				UInt32 group_z = RoundUp(dimension_z, 4);
				ctx.command_list->Dispatch(group_x, group_y, group_z);
			}
		);

		return FroxelLightingResources{ froxel_radiance };
	}

}