#include "TimeManager.h"

namespace Alpha
{
	bool TimeManager::Initialize(const Args& args)
	{
		return true;
	}

	void TimeManager::Finalize()
	{
	}

	void TimeManager::Update(float delta)
	{
		m_elapsed_time += delta;
		m_frame_count++;
	}
}