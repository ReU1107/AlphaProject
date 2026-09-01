#include "CommandListPool.h"
#include "CommandList.h"
#include "RenderCore.h"


namespace Alpha
{
	CommandListPool::CommandListPool()
	{
	}

	CommandListPool::~CommandListPool() = default;

	void CommandListPool::Initialize(const Args& args)
	{
		m_command_lists.resize(args.count);
		m_active_command_lists.reserve(args.count);

		for (UInt32 i = 0; i < args.count; ++i)
		{
			m_command_lists[i] = args.render_core->CreateCommandList(L"CommandListPool::Direct", args.type);
		}
	}

	void CommandListPool::Reset()
	{
		m_active_command_lists.clear();
	}

	CommandList* CommandListPool::AcquireCommandList()
	{
		const size_t index = m_active_command_lists.size();

		if (ALPHA_CHECK_ERROR(index < m_command_lists.size(), L"CommandListPoolのサイズが足りません"))
			return nullptr;

		CommandList* list = m_command_lists[index].get();
		m_active_command_lists.push_back(list);

		return list;
	}

	std::span<CommandList*> CommandListPool::GetActiveCommandLists()
	{
		return {
			m_active_command_lists.data(),
			m_active_command_lists.size()
		};
	}
}