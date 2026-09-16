#pragma once
#include "Core/Common.h"
#include "Core/Container/SlotPalette.h"
#include "RHI/Types.h"

#include "Definitions.h"
#include "RenderGraphPass.h"

#include <functional>

namespace Alpha
{
	class RenderCore;
    class Texture;
}

namespace Alpha::Renderer
{
    struct RenderGraphBuilder;


    using PassSetupFunc = std::function<void(RenderGraphBuilder& builder)>;


    class RenderGraph
    {
    private:

        RenderCore* m_render_core{ nullptr };

        TSlotPalette<RGTextureResource> m_texture_palette{};
        TSlotPalette<RGBufferResource> m_buffer_palette{};

        TUniqueArray<Texture> m_textures{};
        TUniqueArray<Buffer> m_buffers{};

        UInt64 m_texture_handle_counter{ 0 };   // とりあえず
        
        TArray<RenderGraphPass> m_passes{};


    public:
        ~RenderGraph();


        struct Args
        {
            RenderCore* render_core{ nullptr };
        };
		bool Initialize(const Args& args);

        RGTextureHandle CreateTexture(StringView name, const RGTextureDesc& desc);
        RGTextureHandle ImportTexture(StringView name, Texture* texture);

        RGBufferHandle CreateBuffer(StringView name, const RGBufferDesc& desc);
        RGBufferHandle ImportBuffer(StringView name, Buffer* buffer);

        void UpdateImportedTexture(RGTextureHandle handle, Texture* texture);
        void UpdateImportedBuffer(RGBufferHandle handle, Buffer* buffer);

        void AddPass(StringView name, PassSetupFunc&& setup, PassExecuteFunc&& execute);

        Texture* GetTexture(RGTextureHandle handle) const;

        Texture* FindTexture(StringView name) const;

        Buffer* GetBuffer(RGBufferHandle handle) const;

        void Execute(RenderContext& ctx);
 

    };

}