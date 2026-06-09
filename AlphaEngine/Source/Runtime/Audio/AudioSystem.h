#pragma once
#include "Core/Subsystem.h"

namespace Alpha
{
	class AudioSystem : public Subsystem<AudioSystem>
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