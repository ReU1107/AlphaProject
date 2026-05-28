#pragma once
#include <queue>
#include <mutex>
#include <cstdint>
#include "Request.h"
#include "Subsystem.h"

namespace Alpha
{
	class Request;

	class RequestManager
	{
	private:
		using Requests = std::queue<Request*>;
		using Recycles = std::vector<Request*>;
	private:
		std::mutex m_mutex;
		Requests m_requests;	// 関数リクエスト配列
		Recycles m_recycles;	// 使いまわしよう
	public:
		// コンストラクタ
		RequestManager();
		// デストラクタ
		~RequestManager();

	public:
		// 別スレッドで行いたい関数設定
		void PushRequest(const std::function<void(uint32_t)>& function);

		// 別スレッドでやりたい関数設定(クラス関数)
		template<class T>
		void PushRequest(T* object, void(T::* callFunction)(uint32_t));

		// リクエスト取得
		Request* PopRequest();

	private:
		// ヘッダーでのインクルード防止
		void CallThreadGroup();
	};

	template<class T>
	inline void RequestManager::PushRequest(T* object, void(T::* callFunction)(uint32_t))
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		Request* request = nullptr;
		if (!m_recycles.empty())
		{
			request = m_recycles.back();
			m_recycles.pop_back();
			request->SetFunction(std::bind(callFunction, object, std::placeholders::_1));
		}
		else
		{
			request = new Request(std::bind(callFunction, object, std::placeholders::_1));
		}
		m_requests.push(request);
		CallThreadGroup();
	}

}