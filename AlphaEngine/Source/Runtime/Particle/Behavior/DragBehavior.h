#pragma once
#include "Behavior.h"

namespace Alpha::Particle
{
	class DragBehavior : public Behavior
	{
	public:
		~DragBehavior();

		void Execute(UpdateContext& ctx) final;

	};

}