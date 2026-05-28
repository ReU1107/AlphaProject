#pragma once
#include "Core/Object.h"
#include "Context/Snapshot.h"
#include "Context/ParameterContext.h"

namespace Alpha
{
	namespace Animation
	{
		class StateMachine;

	}

	class AnimationComponent : public Object
	{
	private:
		TUnique<Animation::StateMachine> m_root_machine{ nullptr };
		Animation::Snapshot m_current_pose{};
		Animation::ParameterContext m_parameter_context{};
		bool m_is_paused{ false };

	public:
		AnimationComponent();
		~AnimationComponent();

		// 上位層が呼ぶ関数
		void SetParameter(StringView name, float value);
		// 状態（時間・遷移）を進める
		void Update(float dt);     
		// 現在の状態から値を算出する
		void Evaluate();

		bool Play();
		void Pause();

		Animation::StateMachine* GetRootMachine() const { return m_root_machine.get(); }

		const Animation::Snapshot& GetCurrentPose() const { return m_current_pose; }

		void Show();

		void Accept(IVisitor* visitor) override;
	};

}