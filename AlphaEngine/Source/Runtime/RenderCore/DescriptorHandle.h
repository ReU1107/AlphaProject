#pragma once
#include "Core/Types.h"
#include "RHI/Types.h"

namespace Alpha
{
	class DescriptorHeap;

	struct DescriptorHandle
	{
		DescriptorHeap* heap{ nullptr };
		UInt32 index{ 0 };
		ERHIResourceViewType view_type{ ERHIResourceViewType::Unknown };

		bool IsActive() const
		{
			return (heap != nullptr);
		}
		// 
		void Free(UInt32 current_buffer_index);
	};
}