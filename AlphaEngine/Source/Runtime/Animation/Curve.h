#pragma once
#include "Core/Common.h"
#include "Definitions.h"

namespace Alpha::Animation
{


	class Curve
	{
	private:
		TArray<Keyframe> m_keyframes{};
		float m_end_time{ 0.0f };

	public:
		Curve() = default;
		~Curve() = default;

		// キーフレーム追加
		void AddKeyframe(const Keyframe& keyframe);

		// 総キーフレーム数取得
		uint64_t GetKeyframeCount() const;

		const TArray<Keyframe>& GetKeyframes() const { return m_keyframes; }

		float GetEndTime() const { return m_end_time; }

		// キーフレームで設定したフレーム番号順に並び替え
		float Sort();

		// time時の値取得
		float Evaluate(float time) const;

		// 隣り合うキーフレームセット取得
		AdjacentKeyframes GetAdjacentKeyframes(float time) const;


	};

}