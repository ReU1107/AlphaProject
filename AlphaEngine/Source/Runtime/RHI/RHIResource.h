#pragma once
#include "Types.h"

namespace Alpha
{
	// テクスチャ、バッファの基底用
	class RHIResource
	{
	private:
		ERHIResourceState m_current_state{ ERHIResourceState::Common };
		ERHIResourceAccessType m_access{ ERHIResourceAccessType::Static };
		ERHIResourceBindFlags m_flags{ ERHIResourceBindFlags::ShaderResource };
		
	public:
		virtual ~RHIResource() {}

		void SetState(ERHIResourceState state) { m_current_state = state; }
		void SetAccessType(ERHIResourceAccessType access) { m_access = access; }
		void SetFlags(ERHIResourceBindFlags flags) { m_flags = flags; }

		ERHIResourceState GetCurrentState() const { return m_current_state; }

		ERHIResourceBindFlags GetFlags() const { return m_flags; }

		template<typename T>
		T* As()
		{
			return static_cast<T*>(this);
		}

		virtual void* GetNative() const = 0;

	};

}