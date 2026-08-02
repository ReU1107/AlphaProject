#include "AnimationSystem.h"
//#include "Asset/SkeletonHandle.h"

namespace Alpha
{
	//using namespace Animation;

	bool AnimationSystem::Initialize(const Args& args)
	{
		return false;
	}

	void AnimationSystem::Finalize()
	{
	}

	//UInt32 AnimationSystem::RegisterMesh(TShared<SkeletonHandle> handle)
	//{
	//	UInt64 handle_id = handle->GetID();
	//	UInt32 palette_index = 0;

	//	//if (m_skeleton_palette.TryGetIndex(handle_id, &palette_index))
	//	//{
	//	//	return palette_index;
	//	//}

	//	//SkeletonResource& skeleton = m_skeleton_palette.Allocate(handle_id, palette_index);

	//	return palette_index;
	//}

	void AnimationSystem::Update(entt::registry& registry, float delta_time)
	{
		//std::vector<AnimationComponent*> components{};

		//for (auto component : components)
		//{
		//	// アニメーショングラフ計算に必要なデータの更新
		//	component->Update(delta_time);
		//}

		//for (auto component : components)
		//{
		//	// アニメーショングラフの評価
		//	component->Evaluate();
		//}


		//for (auto component : components)
		//{
		//	// スケルトンへの適用
		//	//	pose->ApplyPose(component->GetCurrentPose());
		//}


	}
}