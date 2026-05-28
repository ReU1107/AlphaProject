#pragma once
#include "Types.h"

namespace Alpha
{
	// テクスチャ、バッファの基底用
	class RHIResource
	{
	private:
		ERHIResourceState m_current_state{ ERHIResourceState::Common };

	public:
		virtual ~RHIResource() {}

		// 状態設定
		void SetState(ERHIResourceState state) { m_current_state = state; }
		// 状態取得
		ERHIResourceState GetCurrentState() const { return m_current_state; }

		template<typename T>
		T* As()
		{
			return static_cast<T*>(this);
		}

		virtual void* GetNative() const = 0;

	};

}