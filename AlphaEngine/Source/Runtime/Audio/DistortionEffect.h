#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class AHIEffect;
	struct AHIEffectDesc;
}

namespace Alpha::Audio
{

	struct DistortionParam
	{
		float gain{ 1.0f };      // 歪みの強さ
		float edge{ 0.5f };      // クリップの鋭さ
	};

	class DistortionEffect
	{
	private:
		TUnique<AHIEffect> m_ahi{ nullptr };

	public:
		DistortionEffect();
		~DistortionEffect();
		bool Initialize();
	};
}