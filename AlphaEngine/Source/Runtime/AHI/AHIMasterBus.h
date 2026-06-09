#pragma once
#include "Declarations.h"

namespace Alpha
{
	class AHIMasterBus
	{
	public:
		virtual ~AHIMasterBus() {};

		virtual bool Initialize(const AHIMasterBusDesc& desc) = 0;

	};
}