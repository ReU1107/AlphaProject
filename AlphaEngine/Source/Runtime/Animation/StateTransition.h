#pragma once
#include "Core/Common.h"
#include "Core/Object.h"
#include "Definitions.h"

namespace Alpha::Animation
{
	enum class EConditionType
	{
		All, // すべての条件が真 (AND)
		Any  // いずれかの条件が真 (OR)
	};

	class StateTransition : public Object
	{
	private:
		TArray<Condition> m_conditions{};
		String m_src_name{};
		String m_dest_name{};
		uint32_t m_dest_index{ 0 };	// 遷移先のステートid
		float m_duration{ 0.2f };	// 何秒かけて遷移するか

		EConditionType m_condition_type{ EConditionType::All };

	public:
		void SetDuration(float duration) { m_duration = duration; }

		float GetDuration() const { return m_duration; }

		void SetSrcName(StringView name) { m_src_name = name; }
		void SetDestName(StringView name) { m_dest_name = name; }

		void SetDestIndex(uint32_t index) { m_dest_index = index; }

		uint32_t GetDestIndex() const { return m_dest_index; }

		void AddCondition(const Condition& condition);

		void SetConditionType(EConditionType type) { m_condition_type = type; }

		bool CanTransition(const ParameterContext& ctx) const;

		void Accept(IVisitor* visitor) final;

	};
}