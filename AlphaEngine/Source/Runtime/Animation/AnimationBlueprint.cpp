#include "AnimationBlueprint.h"

#include "StateMachine.h"
#include "Graph.h"
#include "Node.h"

#include <cmath>
#include <memory>
#include <string>
#include <utility>

namespace Alpha::Animation
{
    UInt32 GraphDefinition::AddLeafNode(TShared<Clip> clip)
    {
        const auto index = static_cast<UInt32>(nodes.size());

        nodes.emplace_back(LeafNodeDefinition{ clip });

        return index;
    }

    UInt32 GraphDefinition::AddBlendNode(UInt32 lhs, UInt32 rhs, float alpha)
    {
        const auto index = static_cast<UInt32>(nodes.size());

        nodes.emplace_back(BlendNodeDefinition{ static_cast<Int32>(lhs), static_cast<Int32>(rhs), alpha});

        return index;
    }

    UInt32 StateMachineDefinition::AddState(StringView state_name)
    {
        const auto index = static_cast<UInt32>(states.size());

        StateDefinition definition;
        definition.name = state_name;

        states.emplace_back(std::move(definition));

        return index;
    }

    UInt32 StateMachineDefinition::AddStateMachine(StringView state_name)
    {
        const auto index = static_cast<UInt32>(states.size());

        StateMachineDefinition definition = {};
        definition.name = state_name;

        states.emplace_back(std::move(definition));

        return index;
    }

    StateDefinition* StateMachineDefinition::GetState(UInt32 index)
    {
        if (index >= states.size())
            return nullptr;

        auto value = std::get_if<StateDefinition>(&states[index]);
        return value ? value : nullptr;
    }

    const StateDefinition* StateMachineDefinition::GetState(UInt32 index) const
    {
        if (index >= states.size())
            return nullptr;

        auto value = std::get_if<StateDefinition>(&states[index]);
        return value ? value : nullptr;
    }

    StateMachineDefinition* StateMachineDefinition::GetStateMachine(UInt32 index)
    {
        if (index >= states.size())
            return nullptr;

        auto value = std::get_if<StateMachineDefinition>(&states[index]);
        return value ? value : nullptr;
    }

    const StateMachineDefinition* StateMachineDefinition::GetStateMachine(UInt32 index) const
    {
        if (index >= states.size())
            return nullptr;

        auto value = std::get_if<StateMachineDefinition>(&states[index]);
        return value ? value : nullptr;
    }

    StateDefinitionBase* StateMachineDefinition::GetChild(UInt32 index)
    {
        if (auto state = GetState(index))
            return state;

        return GetStateMachine(index);
    }

    const StateDefinitionBase* StateMachineDefinition::GetChild(UInt32 index) const
    {
        if (auto state = GetState(index))
            return state;

        return GetStateMachine(index);
    }

    namespace
    {
        TUnique<Graph> BuildGraph(const GraphDefinition& definition)
        {
            auto graph = std::make_unique<Graph>();
            TArray<Node*> nodes;
            nodes.reserve(definition.nodes.size());

            // 一度全てのノードを生成
            for (const auto& node_definition : definition.nodes)
            {
                if (const auto leaf = std::get_if<LeafNodeDefinition>(&node_definition))
                {
                    auto node = graph->MakeLeafNode();
                    node->SetClip(leaf->clip);
                    nodes.push_back(node);
                }
                else
                {
                    auto node = graph->MakeBlendNode();
                    node->SetAlpha(std::get<BlendNodeDefinition>(node_definition).alpha);
                    nodes.push_back(node);
                }
            }

            // ブレンド関係の解決
            for (std::size_t i = 0; i < definition.nodes.size(); ++i)
            {
                auto blend = std::get_if<BlendNodeDefinition>(&definition.nodes[i]);
                if (blend)
                    static_cast<BlendNode*>(nodes[i])->SetInputs(nodes[blend->lhs_index], nodes[blend->rhs_index]);
            }


            graph->SetEntry(definition.entry_node_index);
            return graph;
        }

        void ApplySettings(const StateDefinitionBase& definition, State& state)
        {
            state.SetName(definition.name);
            state.SetPlayRate(definition.play_rate);
            state.SetLooping(definition.is_looping);
        }

        void BuildMachine(const StateMachineDefinition& definition, StateMachine& machine)
        {
            ApplySettings(definition, machine);
            machine.SetEntryStateIndex(definition.entry_state_index);

            TArray<State*> states;
            states.reserve(definition.states.size());

            for (std::size_t i = 0; i < definition.states.size(); ++i)
            {
                const auto index = static_cast<UInt32>(i);
                if (const auto child = definition.GetState(index))
                {
                    auto state = machine.MakeState();
                    ApplySettings(*child, *state);
                    state->SetGraph(BuildGraph(child->graph));
                    states.push_back(state);
                }
                else
                {
                    auto child_machine = machine.MakeStateMachine();
                    BuildMachine(*definition.GetStateMachine(index), *child_machine);
                    states.push_back(child_machine);
                }
            }

            // それぞれのステートにおける遷移先の構築
            for (std::size_t i = 0; i < states.size(); ++i)
            {
                for (const auto& transition_definition : definition.GetChild(static_cast<UInt32>(i))->transitions)
                {
                    auto transition = states[i]->MakeTransition(states[transition_definition.dest_index]);

                    transition->SetDuration(transition_definition.duration);
                    transition->SetConditionType(transition_definition.condition_type);

                    for (const auto& condition : transition_definition.conditions)
                    {
                        transition->AddCondition(condition);
                    }
                }
            }

        }
    }

    TUnique<StateMachine> AnimationBlueprint::Instantiate() const
    {
        auto machine = std::make_unique<StateMachine>();
        BuildMachine(m_root, *machine);
        return machine;
    }
}
