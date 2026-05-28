#pragma once
#include "Core/Types.h"

namespace Alpha
{
	class DescriptorHeap;

	struct DescriptorHandle
	{
		DescriptorHeap* heap{ nullptr };
		UInt32 index{ 0 };
		UInt32 num{ 0 };

		bool IsActive() const
		{
			return (heap != nullptr);
		}
	};
}