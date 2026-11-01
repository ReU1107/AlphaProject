#pragma once
#include "Core/Common.h"
#include "../PassResources.h"

namespace Alpha
{
	class RenderCore;
	class PipelineLayout;
}

namespace Alpha::Renderer
{
	class RenderGraph;

	struct FroxelLightingSettings;

	FroxelLightingResources AddFroxelLightingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph, const FroxelLightingSettings& froxel_settings, const ClusterLightResources& cluster_resources);

}
