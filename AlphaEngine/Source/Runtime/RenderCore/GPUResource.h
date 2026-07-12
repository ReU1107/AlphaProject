#pragma once
#include "Core/Common.h"
#include "DescriptorHandle.h"

namespace Alpha
{
	class RHIResource;

	class GPUResource
	{
	protected:
		TUnique<RHIResource> m_rhi{ nullptr };

	public:
		GPUResource();
		~GPUResource();

		RHIResource* GetRHI() const;

		ERHIResourceBindFlags GetFlags() const;

		void SetName(StringView name);

	};

}