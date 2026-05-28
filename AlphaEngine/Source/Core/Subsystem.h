#pragma once
#include "Assertion.h"
#include <utility>
#include <typeinfo>

namespace Alpha
{
	// サブシステムオブジェクトのインターフェース
	// リストでの管理用
	//class ISubsystem
	//{
	//public:
	//	virtual ~ISubsystem() = default;
	//	virtual bool Initialize() { return true; }
	//	virtual void Finalize() {}
	//};


	template<typename T>
	class Subsystem/* : public ISubsystem*/
	{
	private:
		static T* s_instance;
	protected:
		// コンストラクタ
		Subsystem() = default;
		// デストラクタ
		virtual ~Subsystem() = default;
	private:
		// コピー禁止
		Subsystem(const Subsystem<T>&) = delete;
		Subsystem(Subsystem<T>&&) = delete;
		Subsystem& operator = (const Subsystem<T>&) = delete;

	public:
		template<typename... Args>
		static bool Startup(Args&&... args)
		{
			if (s_instance) return false;

			// 引数をそのまま T のコンストラクタに転送
			s_instance = new T();

			if (ALPHA_CHECK_FATAL(s_instance->Initialize(std::forward<Args>(args)...) == false, L"サブシステムの初期化に失敗しました:"))
			{
				delete s_instance;
				s_instance = nullptr;
				return false;
			}
			return true;
		}

		static void Shutdown()
		{
			if (s_instance)
			{
				s_instance->Finalize();
				delete s_instance;
				s_instance = nullptr;
			}
		}

		static T* GetPtr()
		{
			if (s_instance == nullptr)
				ALPHA_LOG_ERROR(L"サブシステムが初期化されていません");

			return s_instance;
		}

		static T& Get()
		{
			if (s_instance == nullptr)
				ALPHA_LOG_ERROR(L"サブシステムが初期化されていません");
			return *s_instance;
		}

	};

	template<typename T>
	T* Subsystem<T>::s_instance = nullptr;
}