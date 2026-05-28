#include "Request.h"

namespace Engine
{
	namespace ThreadSystem
	{
		Request::Request()
			: m_work()
		{
		}

		Request::Request(const std::function<void(uint32_t)>& func)
			: m_work(func)
		{
		}

		Request::~Request()
		{

		}

		void Request::SetFunction(const std::function<void(uint32_t)>& func)
		{
			m_work = func;
		}

		void Request::Excute(uint32_t threadIndex)
		{
			if (m_work)
				m_work(threadIndex);
		}
	}
}