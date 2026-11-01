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
	VolumetricResources AddVolumetricPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph, 
		const VBufferResources& vbuffer_resources, const FroxelLightingResources& froxel_resources);

}