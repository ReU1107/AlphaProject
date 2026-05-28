#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class RHITimeline;

	class Timeline
	{
	private:
		TUnique<RHITimeline> m_rhi{ nullptr };
		UInt64 m_current_value{ 0 };

	public:
		Timeline();
		~Timeline();
		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
		};

		void Initialize(const Args& args);
		RHITimeline* GetRHI() const;

		uint64_t GetCurrentValue() const { return m_current_value; }
		uint64_t GetNextValue() { return ++m_current_value; }

		void WaitCPU();

	};

}