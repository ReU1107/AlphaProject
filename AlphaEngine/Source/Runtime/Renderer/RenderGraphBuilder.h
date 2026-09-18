#pragma once
#include "Definitions.h"


namespace Alpha::Renderer
{
    class RenderGraph;
    struct RenderGraphPass;

    struct RenderGraphBuilder
    {
    private:
		RenderGraph* graph{ nullptr };
		RenderGraphPass* m_pass{ nullptr };

    public:
        RenderGraphBuilder(RenderGraph* graph, RenderGraphPass* pass)
			: graph(graph), m_pass(pass)
        {

        }

        void UsePipeline(RGPipelineHandle handle, ERHIPipelineType type);


        RGTextureHandle ReadTexture(RGTextureHandle handle, ERHIResourceState state);

        RGTextureHandle WriteTexture(RGTextureHandle handle, ERHIResourceState state);

        RGBufferHandle ReadBuffer(RGBufferHandle handle, ERHIResourceState state);

        RGBufferHandle WriteBuffer(RGBufferHandle handle, ERHIResourceState state);


    };

}