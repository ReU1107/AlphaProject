#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;
	class PipelineLayout;
}

namespace Alpha::Renderer
{
	class RenderGraph;

	void AddFroxelLightingPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph);

}
