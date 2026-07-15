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

		virtual void Destroy(UInt32 current_frame_index) = 0;

		void SetName(StringView name);

	};

}