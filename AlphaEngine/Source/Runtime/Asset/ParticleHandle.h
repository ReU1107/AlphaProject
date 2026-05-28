#pragma once

#include "AssetHandle.h"

namespace Alpha
{
	namespace Particle
	{
		class SkeletalClip;
		class MorphClip;
	}

	class ParticleHandle : public AssetHandle
	{
	private:

	public:
		~ParticleHandle();

		static AssetType Type() { return AssetType::Particle; }

		AssetType GetType() const final { return AssetType::Particle; }

		void Accept(IVisitor* visitor) override {}

	};

}
