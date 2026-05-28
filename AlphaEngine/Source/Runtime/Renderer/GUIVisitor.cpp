#include "GUIVisitor.h"
#include <imgui/imgui.h>

#include "Core/String/StringUtil.h"

namespace Alpha
{
	void GUIVisitor::Text(StringView label)
    {
        ImGui::Text("%s", StringUtil::ConvertToUtf8(label).c_str());
    }

    bool GUIVisitor::BeginTreeNode(StringView label)
    {
        return ImGui::TreeNode(StringUtil::ConvertToUtf8(label).c_str());
    }
    
    void GUIVisitor::EndTreeNode()
    {
        ImGui::TreePop();
    }

    bool GUIVisitor::SliderFloat(StringView label, float* value, float min_v, float max_v)
    {
        return ImGui::SliderFloat(StringUtil::ConvertToUtf8(label).c_str(), value, min_v, max_v);
    }

    void GUIVisitor::CheckBox(StringView label, bool* value)
    {
        ImGui::Checkbox(StringUtil::ConvertToUtf8(label).c_str(), value);
    }

    void GUIVisitor::PlotCurve(StringView label, const float* times, const float* values, int count, float current_time)
    {
        ImGui::PlotLines(StringUtil::ConvertToUtf8(label).c_str(), values, count, 0, nullptr, -1.0f, 1.0f, ImVec2(0, 80));
    }

    void GUIVisitor::PlotHistogram(StringView label, const float* values, int count)
    {

    }

    void GUIVisitor::PushIndent()
    {
        ImGui::Indent();
    }

    void GUIVisitor::PopIndent()
    {
        ImGui::Unindent();
    }
}