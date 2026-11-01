#include "ClusterLightCullingPass.h"
#include "RenderCore/Module.h"
#include "../RenderGraph.h"
#include "../RenderGraphBuilder.h"
#include "../RenderSettings.h"
#include "Core/Math/MathUtil.h"

namespace Alpha::Renderer
{
	struct ClusterLightCullingPassConstants
	{
		uint light_cluster;
		uint light_indices;
		uint result;
	};


	ClusterLightResources AddClusterLightCullingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph, const ClusterGridSettings& cluster_settings)
	{
		auto shader = RenderCore::Get().CreateShader(L"Asset/Shader/ClusterLightCullingCS.hlsl", ERHIShaderType::Compute);

		ComputePipeline* pipeline = RenderCore::Get().CreateComputePipeline({
			.pipeline_layout = layout,
			.compute_shader = shader
			}
		);

		UInt32 dimension_x = cluster_settings.grid_dimensions.X();
		UInt32 dimension_y = cluster_settings.grid_dimensions.Y();
		UInt32 dimension_z = cluster_settings.grid_dimensions.Z();

		auto cluster = graph->CreateTexture(L"ClusterLight",
			{
				dimension_x, dimension_y, dimension_z,
				ERHIResourceDimension::Tex3D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RUInt
			}
		);

		auto light_cluster = graph->CreateBuffer(L"LightCluster",
			{
				sizeof(Cluster), sizeof(Cluster) * dimension_x * dimension_y * dimension_z, EBufferPresetType::RWStructured
			}
		);

		auto light_indices = graph->CreateBuffer(L"LightIndices",
			{
				sizeof(UInt32), sizeof(UInt32) * dimension_x * dimension_y * dimension_z * cluster_settings.max_lights_per_cluster, EBufferPresetType::RWStructured
			}
		);
	

		graph->AddPass(L"ClusterLightCulling",
			[=](RenderGraphBuilder& builder)
			{
				builder.WriteTexture(cluster, ERHIResourceState::UnorderedAccess);
				builder.WriteBuffer(light_cluster, ERHIResourceState::UnorderedAccess);
				builder.WriteBuffer(light_indices, ERHIResourceState::UnorderedAccess);
				builder.UsePipeline(pipeline, ERHIPipelineType::Compute);
			},
			[=](RenderGraphContext& ctx)
			{
				auto* result_tex = ctx.graph->GetTexture(cluster);
				auto* light_cluster_buffer = ctx.graph->GetBuffer(light_cluster);
				auto* light_indices_buffer = ctx.graph->GetBuffer(light_indices);

				ClusterLightCullingPassConstants constants = {};
				constants.result = result_tex->GetUAVHandle().index;
				constants.light_cluster = light_cluster_buffer->GetUAVHandle().index;
				constants.light_indices = light_indices_buffer->GetUAVHandle().index;

				auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);
				ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

				UInt32 group_x = RoundUp(dimension_x, 8);
				UInt32 group_y = RoundUp(dimension_y, 8);
				UInt32 group_z = RoundUp(dimension_z, 4);
				ctx.command_list->Dispatch(group_x, group_y, group_z);
			}
		);

		return ClusterLightResources{ cluster, light_cluster, light_indices };

	}

}