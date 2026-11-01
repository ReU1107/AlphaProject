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

	void AddOpaqueShadingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph,
		const ClusterLightResources& cluster_resources, const VBufferResources& vbuffer_resources);
}