#include "ThreadGroup.h"
#include "Request.h"
#include "RequestManager.h"
#include <Windows.h>
#include <cstdint>
#include "../Utility/Utility.h"

namespace Engine
{
	namespace ThreadSystem
	{
		// 
		static bool Startup = false;

		void _stdcall Thread(HANDLE eventHandle, HANDLE compeletedHandle, uint32_t threadIndex)
		{
			while (Startup) 
			{
				Request* request = SingleRequestManager::GetPtr()->PopRequest();
				if (!request)
				{
					char text[256];
					sprintf_s(text, "リクエストがないためスレッドを止めます。 スレッド番号 : %d \n", threadIndex);
					DBGSTR(text);
					// 何もないためこのスレッドは完了しました合図
					SetEvent(compeletedHandle);
					// 処理するものがなく仕事がないならスレッドを止める
					WaitForSingleObject(eventHandle, INFINITE);
					continue;
				}
				char text[256];
				sprintf_s(text, "リクエストを処理します スレッド番号 : %d \n", threadIndex);
				DBGSTR(text);
				request->Excute(threadIndex);
			}
			return;
		}

		ThreadGroup::ThreadGroup()
			: m_maxThreadCount(std::thread::hardware_concurrency() - 5)
		{
			m_maxThreadCount = 3;

			m_wakeUpHandles.resize(m_maxThreadCount);
			m_workerThreads.resize(m_maxThreadCount);
			Startup = true;
			for (uint32_t i = 0; i < m_maxThreadCount; ++i)
			{
				m_wakeUpHandles[i] = CreateEvent(nullptr, false, false, nullptr);
				WorkerThread worker = {};
				worker.completedHandle = CreateEvent(nullptr, false, false, nullptr);
				worker.thread = new std::thread(Thread, m_wakeUpHandles[i], worker.completedHandle, i);
				m_workerThreads[i] = worker;
			}

		}

		ThreadGroup::~ThreadGroup()
		{
			Startup = false;
			for (uint32_t i = 0; i < m_maxThreadCount; i++) 
			{
				SetEvent(m_wakeUpHandles[i]);
				m_workerThreads[i].thread->join();
				delete m_workerThreads[i].thread;
			}
		}

		void ThreadGroup::SetHandle(int32_t index)
		{
			if (index < 0)
			{
				for (uint32_t i = 0; i < m_maxThreadCount; ++i)
				{
					SetEvent(m_wakeUpHandles[i]);
				}
			}
			else
			{
				if (index > m_maxThreadCount)
					index = (int32_t)m_maxThreadCount;
				SetEvent(m_wakeUpHandles[index]);
			}
		}

		void ThreadGroup::WaitAll()
		{
			HANDLE handles[20];
			for (uint32_t i = 0; i <m_maxThreadCount; ++i)
			{
				handles[i] = m_workerThreads[i].completedHandle;
			}

			WaitForMultipleObjects(m_maxThreadCount, handles, true, INFINITE);

		}
		
	}
}
