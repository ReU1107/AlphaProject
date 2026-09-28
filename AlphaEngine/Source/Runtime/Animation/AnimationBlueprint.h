#pragma once
#include "Core/Common.h"

#include "Definitions.h"
#include "StateTransition.h"

#include <variant>

namespace Alpha::Animation
{
    class Clip;
    class StateMachine;
    class StateTransition;

    struct LeafNodeDefinition
    {
        TShared<Clip> clip{};
    };

    struct BlendNodeDefinition
    {
        Int32 lhs_index{ -1 };
        Int32 rhs_index{ -1 };
        float alpha{ 0.0f };
    };

    using NodeDefinition = std::variant<LeafNodeDefinition, BlendNodeDefinition>;

    struct GraphDefinition
    {
        TArray<NodeDefinition> nodes{};
        Int32 entry_node_index{ 0 };

        UInt32 AddLeafNode(TShared<Clip> clip);
        UInt32 AddBlendNode(UInt32 lhs, UInt32 rhs, float alpha = 0.0f);
    };

    struct TransitionDefinition
    {
        TArray<Condition> conditions{};
        Int32 dest_index{ -1 };
        float duration{ 0.2f };
        EConditionType condition_type{ EConditionType::All };
    };

    struct StateDefinitionBase
    {
        String name{};
        float play_rate{ 0.1f };
        bool is_looping{ false };
        TArray<TransitionDefinition> transitions{};
    };

    struct StateDefinition : StateDefinitionBase
    {
        GraphDefinition graph{};
    };

    struct StateMachineDefinition : StateDefinitionBase
    {
        using ChildDefinition = std::variant<StateDefinition, StateMachineDefinition>;

        TArray<ChildDefinition> states{};
        Int32 entry_state_index{ -1 };

        UInt32 AddState(StringView name);
        UInt32 AddStateMachine(StringView name);

        StateDefinition* GetState(UInt32 index);
        const StateDefinition* GetState(UInt32 index) const;

        StateMachineDefinition* GetStateMachine(UInt32 index);
        const StateMachineDefinition* GetStateMachine(UInt32 index) const;

        StateDefinitionBase* GetChild(UInt32 index);
        const StateDefinitionBase* GetChild(UInt32 index) const;
    };

    class AnimationBlueprint
    {
    private:
        StateMachineDefinition m_root{};

    public:
        StateMachineDefinition& GetRootMachine() { return m_root; }
        const StateMachineDefinition& GetRootMachine() const { return m_root; }


        TUnique<StateMachine> Instantiate() const;
    };
}
