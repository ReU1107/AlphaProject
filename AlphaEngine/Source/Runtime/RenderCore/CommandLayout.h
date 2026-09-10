#pragma once
#include "Core/Common.h"
#include "Definitions.h"

namespace Alpha
{
	class CommandLayout
	{
	private:
		TUnique<RHICommandLayout> m_rhi{ nullptr };

	public:
		CommandLayout();
		~CommandLayout();

		void Initialize(const CommandLayoutArgs& args);

		RHICommandLayout* GetRHI() const;
	};
}