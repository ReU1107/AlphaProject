#include "AsyncJob.h"

namespace Engine
{
	AsyncJob::AsyncJob()
		: m_condition()
		, m_mutex()
		, m_stop(false)
		, m_start(false)
	{
	}
	
	AsyncJob::~AsyncJob()
	{
	}
	
	void AsyncJob::Start()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if(m_stop)
			return;
		m_start = true;
		WakeUp();
	}

	void AsyncJob::End()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_start = false;
		WakeUp();
	}
	
	void AsyncJob::WakeUp()
	{
		// この条件変数で待機しているスレッド全てを起動する
		m_condition.notify_all();
	}

	void AsyncJob::Stop()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_stop = true;
	}

	void AsyncJob::WaitToEnd()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock, [this] { return !m_start; });
	}

	void AsyncJob::WaitToStart()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock, [this] { return m_start; });
	}

	bool AsyncJob::Stoped()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_stop;
	}

	//bool AsyncJob::Ended()
	//{
	//	std::lock_guard<std::mutex> lock(m_mutex);
	//	return m_end;
	//}
}