#pragma once
#include "Behavior.h"

namespace Alpha::Particle
{
	class LifeTimeBehavior : public Behavior
	{
	public:
		~LifeTimeBehavior();

		void Setup(const Settings& settings) final;

		void Cleanup() final;

		void Execute(UpdateContext& ctx) final;

	};

}