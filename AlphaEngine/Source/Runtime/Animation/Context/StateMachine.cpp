#include "StateMachine.h"
#include "StateTransition.h"
#include "Snapshot.h"

#include "Core/Math/MathUtil.h"

namespace Alpha::Animation
{
	StateMachine::~StateMachine() = default;

	State* StateMachine::MakeState()
	{
		auto state = std::make_unique<State>();
		auto ptr = state.get();

		m_sub_states.push_back(std::move(state));

		ptr->SetParent(this);
		ptr->SetIndex(m_sub_states.size() - 1);

		return ptr;
	}

	StateMachine* StateMachine::MakeStateMachine()
	{
		auto state = std::make_unique<StateMachine>();
		auto ptr = state.get();

		m_sub_states.push_back(std::move(state));

		ptr->SetParent(this);
		ptr->SetIndex(m_sub_states.size() - 1);

		return ptr;
	}

	void StateMachine::OnUpdate(ParameterContext& ctx, float delta_time)
	{
		if (!m_current_state)
			return;

		m_elapsed_time += delta_time * m_play_rate;

		m_current_state->OnUpdate(ctx, delta_time);

		// 遷移中
		if(m_next_state)
		{
			// 遷移完了
			if (m_elapsed_time >= m_transition_duration)
			{
				m_current_state->OnExit();
				m_current_state = m_next_state;
				m_next_state = nullptr;
			}
			else
			{
				// 遷移中は次のステートも更新する
				m_next_state->OnUpdate(ctx, delta_time);
			}
			return;
		}

		// 遷移条件を満たすのがあるか
		if (auto found = m_current_state->FindValidTransition(ctx))
		{
			uint32_t dest_index = found->GetDestIndex();
			m_transition_duration = found->GetDuration();

			m_next_state = m_sub_states[dest_index].get();
			m_next_state->OnEnter();
			// state_machineが持つ経過時間は遷移用に使う
			m_elapsed_time = 0.0f;
		}

	}

	void StateMachine::OnEvaluate(Snapshot* out_pose)
	{
		if (!m_current_state)
			return;

		if (m_next_state)
		{
			Snapshot lhs_pose = {};
			lhs_pose.bone_transforms.resize(512);
			m_current_state->OnEvaluate(&lhs_pose);
			Snapshot rhs_pose = {};
			rhs_pose.bone_transforms.resize(512);
			m_next_state->OnEvaluate(&rhs_pose);

			float alpha = InverseLerp(0.0f, m_transition_duration, m_elapsed_time);
			
			// Pose同士の補間
			out_pose->Blend(lhs_pose, rhs_pose, alpha);
		}
		else
			m_current_state->OnEvaluate(out_pose);

	}

	void StateMachine::Play(int32_t state_index)
	{
		uint32_t index = state_index;
		if (state_index < 0 || state_index >= m_sub_states.size())
			index = m_entry_state_index;

		// 今処理しているステートがあるなら強制的に終了させる
		if (m_current_state)
		{
			m_current_state->OnExit();
		}

		// ここでの処理は補間なし
		m_current_state = m_sub_states[index].get();
		m_current_state->OnEnter();
	}

	void StateMachine::Stop()
	{
		if (!m_current_state)
			return;
		m_current_state;
	}

	void StateMachine::Accept(IVisitor* visitor)
	{
		if (visitor->BeginTreeNode(L"StateMachine"))
		{
			visitor->Text(std::format(L"States Count: {}", m_sub_states.size()).c_str());

			for (const auto& state : m_sub_states)
			{
				state->Accept(visitor); // 子要素へ
			}

			visitor->EndTreeNode();
		}
	}
}