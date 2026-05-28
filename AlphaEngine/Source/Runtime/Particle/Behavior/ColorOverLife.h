#pragma once
#include "Behavior.h"

namespace Alpha::Particle
{
	class ColorOverLife : public Behavior
	{
	public:
		~ColorOverLife();

		void Execute(UpdateContext& ctx) final;

	};

}