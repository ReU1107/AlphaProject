#pragma once

namespace Alpha::Animation
{
    enum class Interpolation
    {
        Step,   // 補間なし（パッと切り替わる）
        Linear, // 線形補間（Lerp）
        Cubic,  // 立方補間（エルミート/ベジェ：傾きを使用）
    };

	struct Keyframe
	{
		float value{ 0.0f };
		float time{ 0.0f };				// 時間
        Interpolation interpolation{ Interpolation::Linear };

        // 傾きデータ（Cubicの場合に使用）
        float in_tangent{ 0.0f };
        float out_tangent{ 0.0f };
	};

}