#pragma once
#include "State.h"

namespace Alpha::Animation
{

	// 階層型ステートマシン

	class StateMachine : public State
	{
	private:
		State* m_current_state{ nullptr };
		State* m_next_state{ nullptr };

		TUniqueArray<State> m_sub_states{};		// このステートマシンが持つ全ての状態
		uint32_t m_entry_state_index{ 0 };
		float m_transition_duration{ 0.0f };

	public:
		~StateMachine();

		State* MakeState();
		StateMachine* MakeStateMachine();

		void OnUpdate(ParameterContext& ctx, float delta_time) override;

		void OnEvaluate(Snapshot* out_pose) override;

		void Play(int32_t state_index = -1);

		void Stop();

		void Accept(IVisitor* visitor) override;

	};
}