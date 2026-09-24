#pragma once
#include "Core/Common.h"
#include "Definitions.h"

namespace Alpha::Animation
{
	class StateMachine;
	class SkeletonPose;

	struct AnimationComponent
	{
		TUnique<StateMachine> m_state_machine{ nullptr };
		TUnique<SkeletonPose> m_pose{ nullptr };
		Snapshot m_current_pose{};
		ParameterContext m_parameter_context{};
		bool m_is_paused{ false };
	public:
		AnimationComponent();
		~AnimationComponent();
		// 上位層が呼ぶ関数
		void SetParameter(StringView name, float value);

		bool Play();
		void Pause();

		const Snapshot& GetCurrentPose() const { return m_current_pose; }

	};

}