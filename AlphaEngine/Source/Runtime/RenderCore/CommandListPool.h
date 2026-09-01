#pragma once
#include "Core/Common.h"
#include "RHI/Types.h"

#include <span>

namespace Alpha
{

	class RenderCore;
	class CommandList;

	class CommandListPool
	{
	private:
		TUniqueArray<CommandList> m_command_lists{};
		TArray<CommandList*> m_active_command_lists{};

	public:
		CommandListPool();
		~CommandListPool();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			UInt32 count{ 4 };
			ERHICommandType type{ ERHICommandType::Direct };

		};

		void Initialize(const Args& args);

		void Reset();

		CommandList* AcquireCommandList();

		std::span<CommandList*> GetActiveCommandLists();
	};
}