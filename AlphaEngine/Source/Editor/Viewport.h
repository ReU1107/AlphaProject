#pragma once
#include "Core/Common.h"
#include "Panel.h"

namespace Alpha
{
    class Texture;

}

namespace Alpha::Editor
{

    class Viewport : public Panel
    {
    private:
        TUnique<Texture> m_render_target{};

        UInt32 m_width{};
        UInt32 m_height{};

    public:
        void Draw() final;

        Texture* GetRenderTarget() const
        {
            return m_render_target.get();
        }

        UInt32 GetWidth() const
        {
            return m_width;
        }

        UInt32 GetHeight() const
        {
            return m_height;
        }

    private:
        void Resize(UInt32 width, UInt32 height);

        Texture* GetRenderTarget();

    };

}