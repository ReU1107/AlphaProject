#include "StateTransition.h"

namespace Alpha::Animation
{
    void StateTransition::AddCondition(const Condition& condition)
    {
        m_conditions.push_back(condition);
    }

    bool StateTransition::CanTransition(const ParameterContext& ctx) const
	{
		if (m_conditions.empty())
			return true;

        switch (m_condition_type)
        {
        case EConditionType::All:
            for (const auto& condition : m_conditions)
            {
                // 1つでもダメなら偽
                if (!condition.Evaluate(ctx)) 
                    return false;
            }
        return true;
        case EConditionType::Any:
            for (const auto& condition : m_conditions)
            { 
                // 1つでも良ければ真
                if (condition.Evaluate(ctx)) 
                    return true;
            }
        return false;        
        default:
            break;
        }

		return false;
	}

    void StateTransition::Accept(IVisitor* visitor)
    {
        std::wstring label = std::format(L"{} -> {}", m_src_name.c_str(), m_dest_name.c_str());

        if (visitor->BeginTreeNode(label.c_str()))
        {
            visitor->SliderFloat(L"Duration", &m_duration, 0.0f, 5.0f);

            for (auto& cond : m_conditions)
            {
                visitor->Text(std::format(L"Cond: {} {} {:.2f}",
                    cond.parameter_name.c_str(), (int)cond.compare_type, cond.threshold).c_str());
            }
            visitor->EndTreeNode();
        }
    }
}