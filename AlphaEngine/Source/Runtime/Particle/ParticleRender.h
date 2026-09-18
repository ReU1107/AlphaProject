#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;

	class Buffer;
	class PipelineLayout;
	class CommandLayout;
	class ComputePipeline;
	class Shader;


}

namespace Alpha::Renderer
{
	class RenderGraph;

}

namespace Alpha::Particle
{
	class ParticleSystem;
	void AddParticleRenderPass(RenderCore* render_core, PipelineLayout* layout, ParticleSystem* system, Renderer::RenderGraph* graph);


}