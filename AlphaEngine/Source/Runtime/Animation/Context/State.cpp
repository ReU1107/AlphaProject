#include "State.h"
#include "StateTransition.h"
#include "Graph.h"

namespace Alpha::Animation
{
	State::State() = default;
	State::~State() = default;

	void State::SetGraph(TUnique<Graph> graph)
	{
		m_graph = std::move(graph);
	}

	StateTransition* State::MakeTransition(State* dest_state)
	{
		// 同じ親を持つ
		if (m_parent != dest_state->m_parent)
			return nullptr;

		auto transition = std::make_unique<StateTransition>();

		auto ptr = transition.get();

		m_transitions.push_back(std::move(transition));

		ptr->SetSrcName(GetName());
		ptr->SetDestName(dest_state->GetName());
		ptr->SetDestIndex(dest_state->GetIndex());

		return ptr;
	}

	StateTransition* State::FindValidTransition(ParameterContext& ctx)
	{
		for (auto& transition : m_transitions)
		{
			// 遷移条件チェック
			if (transition->CanTransition(ctx))
				return transition.get();
		}
		return nullptr;
	}

	void State::OnEnter()
	{
		m_elapsed_time = 0.0f;

		m_is_playing = true;
	}

	void State::OnUpdate(ParameterContext& ctx, float delta_time)
	{
		if (!m_is_playing)
			return;

		// 時間の更新
		m_elapsed_time += delta_time * m_play_rate;

		// 一旦ループは後回し
		if (m_is_looping)
		{
		
		}

		m_graph->Update(delta_time);
	}

	void State::OnEvaluate(Snapshot* out_pose)
	{
		m_graph->Evaluate(m_elapsed_time, out_pose);
	}

	void State::OnExit()
	{
		m_is_playing = false;
	}

	void State::Accept(IVisitor* visitor)
	{
		// 各ステートの名前（Walk, Run等）でノードを作成
		if (visitor->BeginTreeNode(GetName()))
		{
			visitor->SliderFloat(L"PlayRate", &m_play_rate, 0.01f, 5.0f);

			// 遷移条件（Transitions）のリストもツリーに含める
			if (visitor->BeginTreeNode(L"Transitions"))
			{
				for (auto& transition : m_transitions)
				{
					transition->Accept(visitor);
				}
				visitor->EndTreeNode();
			}

			if (m_graph)
				m_graph->Accept(visitor);

			visitor->EndTreeNode();
		}
	}

}