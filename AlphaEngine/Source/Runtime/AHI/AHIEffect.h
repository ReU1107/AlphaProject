#pragma once
#include "Declarations.h"

namespace Alpha
{
	class AHIEffect
	{
	public:
		virtual ~AHIEffect() {};
		virtual bool Initialize(const AHIEffectDesc& desc) = 0;

	};
}