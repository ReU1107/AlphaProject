#pragma once
#include "Core/Types.h"
#include "Core/String/String.h"

#include <format>
#include <iostream>
#include <sstream>

namespace Alpha
{
	class IVisitor
	{
	public:
		virtual ~IVisitor() = default;
		// 基本的なテキスト表示
		virtual void Text(StringView label) = 0;

		// 階層構造（ツリー）の開始・終了
		// 戻り値が true の時だけ中身（子要素）を走査する
		virtual bool BeginTreeNode(StringView label) = 0;
		virtual void EndTreeNode() = 0;

		// 数値編集の抽象化
		virtual bool SliderFloat(StringView label, float* value, float min_v, float max_v) = 0;

		virtual void CheckBox(StringView label, bool* value) = 0;

		// times: 時間軸(x), values: 値(y)
		virtual void PlotCurve(StringView label, const float* times, const float* values, int count, float current_time) = 0;

		// 現在の値を履歴として表示する（実行時の推移用）
		virtual void PlotHistogram(StringView label, const float* values, int count) = 0;

		// インデント操作
		virtual void PushIndent() = 0;
		virtual void PopIndent() = 0;

	};
}
