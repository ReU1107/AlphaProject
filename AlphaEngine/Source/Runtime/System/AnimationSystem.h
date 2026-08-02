#pragma once
#include "Core/Common.h"
#include "Core/Container/SlotPalette.h"
#include "Definitions.h"
#include <entt/entt.hpp>

namespace Alpha
{
	class SkeletonHandle;

	class AnimationSystem
	{
	private:
		//TSlotPalette<Animation::SkeletonResource> m_skeleton_palette{};


	public:


		struct Args
		{
		};

		bool Initialize(const Args& args);
		void Finalize();


		//UInt32 RegisterMesh(TShared<SkeletonHandle> handle);

		void Update(entt::registry& registry, float delta_time);

	};

}