#include "AnimationSystem.h"
#include "AnimationComponent.h"

namespace Alpha
{
	bool AnimationSystem::Initialize(const Args& args)
	{
		return false;
	}

	void AnimationSystem::Finalize()
	{
	
	}

	void AnimationSystem::Update(float delta_time)
	{
		std::vector<AnimationComponent*> components{};

		for (auto component : components)
		{
			// アニメーショングラフ計算に必要なデータの更新
			component->Update(delta_time);
		}

		for (auto component : components)
		{
			// アニメーショングラフの評価
			component->Evaluate();
		}


		for (auto component : components)
		{
			// スケルトンへの適用
			//	pose->ApplyPose(component->GetCurrentPose());
		}


	}
}