#include "Viewport.h"
#include <imgui/imgui.h>

namespace Alpha::Editor
{
	void Viewport::Draw()
	{
        ImGui::Begin(
            "Viewport",
            nullptr,
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse
        );

        const ImVec2 size = ImGui::GetContentRegionAvail();

        const UInt32 width = static_cast<UInt32>(size.x);
        const UInt32 height = static_cast<UInt32>(size.y);

        if (width > 0 && height > 0)
        {
            if (width != m_width || height != m_height)
                Resize(width, height);

            ImGui::Image(m_render_target.get(), size);

        }

        ImGui::End();

	}

    void Viewport::Resize(UInt32 width, UInt32 height)
    {
    }



    Texture* Viewport::GetRenderTarget()
    {
        return m_render_target.get();
    }
}