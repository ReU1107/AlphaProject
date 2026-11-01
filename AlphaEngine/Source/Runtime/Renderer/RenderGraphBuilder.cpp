#include "RenderGraphBuilder.h"
#include "RenderGraph.h"
#include "RenderGraphPass.h"

namespace Alpha::Renderer
{
	void RenderGraphBuilder::UsePipeline(RGPipelineHandle handle, ERHIPipelineType type)
	{
		m_pass->pipeline = { handle,type };
	}

	/*
	* ReadやWriteのに基づいたステートになっているかをチェックする
	*/

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
		m_pass->buffers.push_back({ handle, state });

		return handle;
	}

	RGBufferHandle RenderGraphBuilder::WriteBuffer(RGBufferHandle handle, ERHIResourceState state)
	{
		m_pass->buffers.push_back({ handle, state });

		return handle;
	}
}