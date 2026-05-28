#pragma once
#include "Core/Subsystem.h"

namespace Alpha
{
	class AnimationSystem : public Subsystem<AnimationSystem>
	{
	public:


		struct Args
		{
		};

		bool Initialize(const Args& args);
		void Finalize();


		void Update(float delta_time);

	};

}