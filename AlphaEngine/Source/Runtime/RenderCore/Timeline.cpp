#include "Timeline.h"
#include "RenderHardwareInterface.h"
#include "RHI/RHITimeline.h"

namespace Alpha
{
	Timeline::Timeline() = default;
	Timeline::~Timeline() = default;

	void Timeline::Initialize(const Args& args)
	{
		RHITimelineDesc desc = {};
		m_rhi = args.rhi->CreateTimeline(desc);

		m_frame_values.resize(args.buffer_num);
	}

	RHITimeline* Timeline::GetRHI() const
	{
		return m_rhi.get();
	}

	UInt64 Timeline::GetNextValue()
	{
		++m_global_value;

		m_frame_values[m_current_buffer_index] = m_global_value;

		return m_global_value;
	}

	UInt64 Timeline::GetCurrentValue() const
	{
		return m_global_value;
	}

	void Timeline::WaitCPUCurrentFrame(UInt32 next_swapchain_index)
	{
		UInt64 target_value = m_frame_values[next_swapchain_index];
		m_rhi->WaitCPU(target_value);

		m_current_buffer_index = next_swapchain_index;
	}

	void Timeline::WaitCPU(UInt64 value)
	{
		m_rhi->WaitCPU(value);
	}

	void Timeline::WaitCPULatest()
	{
		m_rhi->WaitCPU(m_global_value);

	}

}
