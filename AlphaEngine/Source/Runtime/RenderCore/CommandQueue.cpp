#include "CommandQueue.h"
#include "CommandList.h"
#include "Timeline.h"
#include "RenderHardwareInterface.h"
#include "RHI/RHICommandQueue.h"

namespace Alpha
{
	CommandQueue::CommandQueue() = default;
	CommandQueue::~CommandQueue() = default;

	void CommandQueue::Initialize(const Args& args)
	{
		RHICommandQueueDesc desc = {};
		desc.type = args.type;
		m_rhi = args.rhi->CreateCommandQueue(desc);
	}

	RHICommandQueue* CommandQueue::GetRHI() const
	{
		return m_rhi.get();
	}

	void CommandQueue::Execute(CommandList* command_list)
	{
		m_rhi->ExecuteCommandBuffer(command_list->GetRHI());
	}

	void CommandQueue::Execute(CommandList** command_lists, UInt32 count)
	{
		//RHICommandBuffer** rhi_command_buffers = 
		//for (UInt32 i = 0; i < count; ++i)
		//{
		//	rhi_command_buffers[i] = command_lists[i]->GetRHI();
		//}
		//m_rhi->ExecuteCommandBuffers(rhi_command_buffers, count);
		//AlphaDeleteArray(rhi_command_buffers);
	}

	UInt64 CommandQueue::Signal(Timeline* timeline)
	{
		UInt64 fence_value = timeline->GetNextValue();

		m_rhi->Signal(timeline->GetRHI(), fence_value);

		return fence_value;
	}

	void CommandQueue::Wait(Timeline* timeline)
	{
		m_rhi->Wait(timeline->GetRHI(), timeline->GetCurrentValue());
	}

	void CommandQueue::Wait(Timeline* timeline, UInt64 signal_value)
	{
		m_rhi->Wait(timeline->GetRHI(), signal_value);
	}
}