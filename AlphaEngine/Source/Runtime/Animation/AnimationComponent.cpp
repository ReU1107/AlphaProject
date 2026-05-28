#include "AnimationComponent.h"
#include "Context/StateTransition.h"
#include "Context/StateMachine.h"
#include "Context/Graph.h"

namespace Alpha
{
	using namespace Animation;

	AnimationComponent::AnimationComponent()
	{
		m_root_machine = std::make_unique<StateMachine>();
		m_current_pose.bone_transforms.resize(512);
	}

	AnimationComponent::~AnimationComponent() = default;

	void AnimationComponent::SetParameter(StringView name, float value)
	{
		m_parameter_context.Set(name, value);
	}

	void AnimationComponent::Update(float dt)
	{
		if (m_is_paused)
			return;

		m_root_machine->OnUpdate(m_parameter_context, dt);
	}

	void AnimationComponent::Evaluate()
	{
		m_root_machine->OnEvaluate(&m_current_pose);
	}

	bool AnimationComponent::Play()
	{
		m_root_machine->Play(0);
		return true;
	}

	void AnimationComponent::Pause()
	{
		m_is_paused = true;
	}

	void AnimationComponent::Show()
	{

	}

	void AnimationComponent::Accept(IVisitor* visitor)
	{
		if (visitor->BeginTreeNode(L"AnimationComponent"))
		{
			visitor->CheckBox(L"IsPaused", &m_is_paused);
			if (visitor->BeginTreeNode(L"Parameters"))
			{
				for (auto& [name, value] : m_parameter_context.parameter_set)
				{
					float v = value;
					if (visitor->SliderFloat(name.c_str(), &v, 0.0f, 10.0f))
					{
						this->SetParameter(name, v);
					}
				}
				visitor->EndTreeNode();
			}
			m_root_machine->Accept(visitor);

			visitor->EndTreeNode();
		}
	}
}