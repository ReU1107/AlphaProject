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
	}

	RHITimeline* Timeline::GetRHI() const
	{
		return m_rhi.get();
	}

	void Timeline::WaitCPU()
	{
		m_rhi->WaitCPU(m_current_value);
	}
}
