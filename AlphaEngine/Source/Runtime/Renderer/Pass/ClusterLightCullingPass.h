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
	struct ClusterGridSettings;

	ClusterLightResources AddClusterLightCullingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph, const ClusterGridSettings& cluster_settings);

}
