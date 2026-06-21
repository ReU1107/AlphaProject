#include "Curve.h"
#include <algorithm>
#include "Core/Math/MathUtil.h"

namespace Alpha::Animation
{
	uint64_t Curve::GetKeyframeCount() const
	{
		return m_keyframes.size();
	}

	void Curve::AddKeyframe(const Keyframe& keyframe)
	{
		m_keyframes.push_back(keyframe);
	}

	float Curve::Sort()
	{
		std::sort(
			m_keyframes.begin(), m_keyframes.end(),
			[](const Keyframe& lhs, const Keyframe& rhs)
			{
				return lhs.time < rhs.time;
			}
		);

		m_end_time = m_keyframes.back().time;

		return m_end_time;
	}

	float Curve::Evaluate(float time) const
	{
		AdjacentKeyframes keyframes = GetAdjacentKeyframes(time);

		if (keyframes.is_empty)
			return 0.0f;

		if (keyframes.is_single)
			return keyframes.lhs.value;

		const float lhs_time = keyframes.lhs.time;
		const float rhs_time = keyframes.rhs.time;

		float t = (time - lhs_time) / (rhs_time - lhs_time);

		const float lhs_value = keyframes.lhs.value;
		const float rhs_value = keyframes.rhs.value;

		// lhs～rhs間の補完方法はlhsに
		Interpolation interpolation = keyframes.lhs.interpolation;

		switch (interpolation)
		{
		case Interpolation::Step	: return lhs_value;
		case Interpolation::Linear	: return Lerp(lhs_value, rhs_value, t);
		case Interpolation::Cubic	: return Lerp(lhs_value, rhs_value, t);
		default:
			break;
		}

		return lhs_value;
	}
	
	AdjacentKeyframes Curve::GetAdjacentKeyframes(float time) const
	{
		AdjacentKeyframes result = {};
		result.is_single = false;
		result.is_empty = false;

		if (m_keyframes.empty())
		{
			result.is_empty = true;
			return result;
		}

		uint32_t keyframe_num = static_cast<uint32_t>(m_keyframes.size());
		// キーフレームが1つしかないなら
		if (keyframe_num == 1)
		{
			result.is_single = true;
			result.lhs = m_keyframes[0];
			result.rhs = m_keyframes[0];
			result.s = 0.0f;

			return result;
		}
		// 一番最後のキーフレームの時間を超えていたら
		if (time >= m_end_time)
		{
			result.is_single = true;
			result.lhs = m_keyframes.back();
			result.rhs = m_keyframes.back();
			result.s = 1.0f;

			return result;
		}

		/*
		KeyframeがSortされている(time順に並んでいる)仮定で動作するので2分探索ができる
		*/
		uint32_t left = 0;
		uint32_t middle = 0;
		uint32_t right = keyframe_num - 1;
		do
		{
			middle = (left + right) / 2;
			// middleの時間の方が大きい≒timeは真ん中より前にある≒最大値を減らす
			if (m_keyframes.at(middle).time > time)
			{
				right = middle;
			}
			else
			{
				left = middle;
			}

		} while (left + 1 < right);

		result.lhs = m_keyframes.at(left);
		result.rhs = m_keyframes.at(right);

		result.s = ((time - result.lhs.time) / (result.rhs.time - result.lhs.time));


		return result;
	}
}