#pragma once
#include "Core/IVisitor.h"


namespace Alpha
{

    class GUIVisitor : public IVisitor 
    {
    public:
        void Text(StringView label) override;

        bool BeginTreeNode(StringView label) override;

        void EndTreeNode() override;

        bool SliderFloat(StringView label, float* value, float min_v, float max_v) override;

        void CheckBox(StringView label, bool* value) override;

        // times: 時間軸(x), values: 値(y)
        void PlotCurve(StringView label, const float* times, const float* values, int count, float current_time) override;

        // 現在の値を履歴として表示する（実行時の推移用）
        void PlotHistogram(StringView label, const float* values, int count) override;


        void PushIndent() override;
        void PopIndent() override;
    };

}