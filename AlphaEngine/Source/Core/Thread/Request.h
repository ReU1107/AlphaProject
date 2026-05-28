#pragma once
#include <functional>
#include <cstdint>

namespace Engine
{
	namespace ThreadSystem
	{
		// 仕事内容(非同期での)別名AsyncJob
		class Request
		{
		private:
			using Work = std::function<void(uint32_t)>;
		private:
			Work m_work;
		public:
			// コンストラクタ
			Request();
			Request(const std::function<void(uint32_t)>& func);

			// デストラクタ
			~Request();
		public:
			//
			void SetFunction(const std::function<void(uint32_t)>& func);

			// 実行
			void Excute(uint32_t threadIndex);
		};
	}
}