#pragma once
#include "Core/Types.h"
#include "ParameterContext.h"

namespace Alpha::Animation
{
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
			case ECompareType::If		: return (context.Get(parameter_name) > 0.5f);
			case ECompareType::IfNot	: return (context.Get(parameter_name) < 0.5f);
			case ECompareType::Greater	: return (context.Get(parameter_name) > threshold);
			case ECompareType::Less		: return (context.Get(parameter_name) < threshold);
			case ECompareType::Equals	: return (context.Get(parameter_name) == threshold);
			case ECompareType::NotEqual	: return (context.Get(parameter_name) != threshold);
			default:
				break;
			}

			return false;
		}
	};

}