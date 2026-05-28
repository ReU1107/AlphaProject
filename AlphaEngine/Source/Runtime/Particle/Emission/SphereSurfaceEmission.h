#pragma once
#include "Emission.h"

namespace Alpha::Particle
{
	class SphereSurfaceEmission : public Emission
	{
	private:
		float m_radius{ 5.0f };

	public:

		void Setup(const Settings& settings) final;

		void Cleanup() final;

		void Emit(EmitContext& ctx) final;

	};
}