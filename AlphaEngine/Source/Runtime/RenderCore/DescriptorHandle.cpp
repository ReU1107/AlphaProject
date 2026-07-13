#include "DescriptorHandle.h"
#include "DescriptorHeap.h"

namespace Alpha
{
	void DescriptorHandle::Free(UInt32 current_buffer_index)
	{
		auto h = heap;

		h->Free(*this, current_buffer_index);
	}
}