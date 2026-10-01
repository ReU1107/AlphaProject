#pragma once
#include "Declarations.h"
#include "Types.h"

namespace Alpha
{
	class RHIQueryHeap
	{
	private:
		ERHIQueryType m_type{ ERHIQueryType::Timestamp };

	public:
		virtual ~RHIQueryHeap() {}

		virtual bool Initialize(const RHIQueryHeapDesc& desc) = 0;

		void SetType(ERHIQueryType type) { m_type = type; }

		ERHIQueryType GetType() const { return m_type; }
	};

}

