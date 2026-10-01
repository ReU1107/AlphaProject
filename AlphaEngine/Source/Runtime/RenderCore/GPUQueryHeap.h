#pragma once
#include "Core/Common.h"
#include "DescriptorHandle.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class RHIQueryHeap;

	class GPUQueryHeap
	{
	protected:
		TUnique<RHIQueryHeap> m_rhi{ nullptr };

	public:
		GPUQueryHeap();
		~GPUQueryHeap();

		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			ERHIQueryType type{  };
			UInt32 count{ 256 };

		};
		void Initialize(const Args& args);

		RHIQueryHeap* GetRHI() const;

	};

}