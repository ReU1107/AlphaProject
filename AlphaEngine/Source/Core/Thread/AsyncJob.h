#pragma once
#include <mutex>

namespace Engine
{
	class AsyncJob
	{
	private:
		std::condition_variable m_condition;	// 別スレッド待機用条件変数
		std::mutex m_mutex;						// 
		
		bool m_stop;							// スレッドのループ終了
		bool m_start;								// 待機終了

	public:
		// コンストラクタ
		AsyncJob();
		// デストラクタ
		virtual ~AsyncJob();
		// 処理開始通知
		void Start();
		// 処理終了通知
		void End();

		// 待機中のスレッド実行
		void WakeUp();

		void Stop();

		// 終了通知待機
		void WaitToEnd();

	protected:
		// 開始通知待機
		void WaitToStart();
		// 
		bool Stoped();

	};
}
