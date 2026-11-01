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

	VBufferResources AddVBufferPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph);

}