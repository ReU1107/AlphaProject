#pragma once
#include "Behavior.h"

namespace Alpha::Particle
{
	class CurlNoiseBehavior : public Behavior
	{
	public:
		~CurlNoiseBehavior();

		void Execute(UpdateContext& ctx) final;

	};

}