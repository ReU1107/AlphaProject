#include "DescriptorHandle.h"
#include "DescriptorHeap.h"

namespace Alpha
{
	void DescriptorHandle::Free()
	{
		if (heap)
			heap->Free(*this);
	}
}