#pragma once
#include "Declarations.h"

namespace Alpha
{
	class AHISubmixBus
	{
	public:
		virtual ~AHISubmixBus() {};

		virtual bool Initialize(const AHISubmixBusDesc& desc) = 0;

	};
}