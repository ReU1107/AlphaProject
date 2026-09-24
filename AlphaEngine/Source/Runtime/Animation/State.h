#pragma once
#include "Core/Common.h"
#include "Core/Object.h"

namespace Alpha::Animation
{
	class StateTransition;
	class Graph;
	struct Snapshot;
	struct ParameterContext;

	class State : public Object
	{
	protected:
		TUnique<Graph> m_graph{};				// このステートで表現するア
		State* m_parent{ nullptr };						// 
		TUniqueArray<StateTransition> m_transitions{};	// 遷移先一覧
		float m_elapsed_time{ 0.0f };					// 経過時間
		float m_play_rate{ 0.1f };						// 再生速度
		uint32_t m_index{ 0 };							// 親からみて何番目か
		bool m_is_looping{ false };						// 
		bool m_is_playing{ false };

	public:
		State();
		virtual ~State();

		StateTransition* MakeTransition(State* dest_state);
		
		void SetGraph(TUnique<Graph> graph);

		void SetParent(State* state) { m_parent = state; }

		void SetPlayRate(float play_rate) { m_play_rate = play_rate; }

		void SetLooping(bool is_looping) { m_is_looping = is_looping; }

		void SetIndex(uint32_t index) { m_index = index; }

		uint32_t GetIndex() const { return m_index; }

		StateTransition* FindValidTransition(ParameterContext& ctx);

		virtual void OnEnter();

		virtual void OnUpdate(ParameterContext& ctx, float delta_time);

		virtual void OnEvaluate(Snapshot* out_pose);

		virtual void OnExit();

		void Accept(IVisitor* visitor) override;

	};
}