#include "RenderGraphBuilder.h"
#include "RenderGraph.h"
#include "RenderGraphPass.h"

namespace Alpha::Renderer
{
	void RenderGraphBuilder::UsePipeline(RGPipelineHandle handle, ERHIPipelineType type)
	{
		m_pass->pipeline = { handle,type };
	}

	RGTextureHandle RenderGraphBuilder::ReadTexture(RGTextureHandle handle, ERHIResourceState state)
	{
		m_pass->textures.push_back({ handle, state });

		return handle;
	}

	RGTextureHandle RenderGraphBuilder::WriteTexture(RGTextureHandle handle, ERHIResourceState state)
	{
		m_pass->textures.push_back({ handle, state });

		return handle;
	}

	RGBufferHandle RenderGraphBuilder::ReadBuffer(RGBufferHandle handle, ERHIResourceState state)
	{
		return RGBufferHandle();
	}

	RGBufferHandle RenderGraphBuilder::WriteBuffer(RGBufferHandle handle, ERHIResourceState state)
	{
		return RGBufferHandle();
	}
}