#pragma once
#include "Declarations.h"
#include "Types.h"

namespace Alpha
{
	class RHIDescriptorHeap
	{
	private:
		ERHIDescriptorType m_type{ ERHIDescriptorType::SRV_UAV_CBV };

	public:
		virtual ~RHIDescriptorHeap() {}

		virtual bool Initialize(const RHIDescriptorHeapDesc& desc) = 0;

		virtual bool CreateView(const RHIResourceViewDesc& desc) = 0;

		void SetType(ERHIDescriptorType type) { m_type = type; }

		ERHIDescriptorType GetType() const { return m_type; }
	};

}