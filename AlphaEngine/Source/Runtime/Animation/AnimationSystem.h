#pragma once
#include "Core/Common.h"
#include "Core/Subsystem.h"
#include "Core/Container/SlotPalette.h"
#include "Definitions.h"

namespace Alpha
{
	class SkeletonHandle;

	class AnimationSystem : public Subsystem<AnimationSystem>
	{
	private:
		TSlotPalette<Animation::SkeletonResource> m_skeleton_palette{};


	public:


		struct Args
		{
		};

		bool Initialize(const Args& args);
		void Finalize();


		UInt32 RegisterMesh(TShared<SkeletonHandle> handle);

		void Update(float delta_time);

	};

}