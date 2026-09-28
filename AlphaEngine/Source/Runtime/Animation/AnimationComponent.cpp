#include "AnimationComponent.h"
#include "StateMachine.h"
#include "SkeletonPose.h"

namespace Alpha::Animation
{
	AnimationComponent::AnimationComponent() = default;
	AnimationComponent::~AnimationComponent() = default;

	void AnimationComponent::SetParameter(StringView name, float value)
	{
		m_parameter_context.Set(name, value);
	}

	bool AnimationComponent::Play()
	{
		m_state_machine->Play();
		m_is_paused = false;
		return true;
	}

	void AnimationComponent::Pause()
	{
		m_is_paused = true;
	}

}