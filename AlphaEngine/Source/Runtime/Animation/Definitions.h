#pragma once
#include "Core/Common.h"

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

	struct AdjacentKeyframes
	{
		Keyframe lhs{};			// 
		Keyframe rhs{};			// 
		float s{ 0.0f };		// 2つのキーフレーム間の割合 s : (1 -s)
		bool is_empty{ false };		// そもそもキーフレームが存在しない
		bool is_single{ false };	// キーフレームが一つしかないため補間不要(lhsのみ使用してください)

	};

	struct ParameterContext
	{
		TStringMap<float> parameter_set{};

		void Set(StringView name, float value)
		{
			parameter_set[name] = value;
		}

		float Get(StringView name) const
		{
			const auto& found = parameter_set.find(name);
			if (found == parameter_set.cend())
				return 0.0f;

			return found->second;
		}

		uint32_t Size() const
		{
			return static_cast<uint32_t>(parameter_set.size());
		}
	};

	enum class ECompareType
	{
		If,			// パラメータ値が true のとき、条件は true になります。
		IfNot,		// パラメータ値が false のとき、条件は true になります。
		Greater,	// パラメータ値が threshold より大きいとき、条件は true になります。
		Less,		// パラメータ値が threshold より小さいとき、条件は true になります。
		Equals,		// パラメータ値が threshold の値と等しいとき、条件は true になります。
		NotEqual,	// パラメータ値が threshold の値と等しくないとき、条件は true になります。
	};

	struct Condition
	{
		String parameter_name{};			// パラメーター
		float threshold{ 0.0f };				// しきい値(比較対象)
		ECompareType compare_type{ ECompareType::If };	// 比較方法

		// 比較する
		bool Evaluate(const ParameterContext& context) const
		{
			switch (compare_type)
			{
			case ECompareType::If: return (context.Get(parameter_name) > 0.5f);
			case ECompareType::IfNot: return (context.Get(parameter_name) < 0.5f);
			case ECompareType::Greater: return (context.Get(parameter_name) > threshold);
			case ECompareType::Less: return (context.Get(parameter_name) < threshold);
			case ECompareType::Equals: return (context.Get(parameter_name) == threshold);
			case ECompareType::NotEqual: return (context.Get(parameter_name) != threshold);
			default:
				break;
			}

			return false;
		}
	};

	enum class EConstraintMode
	{
		None,	// 制限なし
		Hinge,	// ヒンジ制約:1軸制限（膝・肘用)
		Cone,	// 円錐制約:肩、股関節用
	};

	struct Constraint
	{
		EConstraintMode mode{ EConstraintMode::None };

		// 制約軸
		Vector3 rotation_axis{ Vector3::ZAxis };

		float min_angle{ -PI };
		float max_angle{ PI };
		float cone_angle{ PI };
	};

	struct IKLink
	{
		uint32_t index{ 0 };
		uint32_t next_index{ 0 };
		float length{ 0.0f };
		Vector3 dir{ Vector3::XAxis };

		Constraint constraint{};
	};

	struct Snapshot
	{
		TArray<Transform> bone_transforms{}; // 骨のポーズ
		TArray<float> morph_weights{};       // モーフターゲットのウェイト


		void Blend(const Snapshot& lhs, const Snapshot& rhs, float alpha);
	};


	class Skeleton;

	struct SkeletonResource
	{
		//TUnique<Skeleton> instance{ nullptr };
	};

}