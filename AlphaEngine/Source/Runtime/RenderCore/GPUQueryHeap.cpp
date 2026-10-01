#include "GPUQueryHeap.h"
#include "RenderHardwareInterface.h"

#include "RHI/RHIQueryHeap.h"


namespace Alpha
{
	GPUQueryHeap::GPUQueryHeap() = default;
	GPUQueryHeap::~GPUQueryHeap() = default;

	void GPUQueryHeap::Initialize(const Args& args)
	{
		RHIQueryHeapDesc desc = {
			.count = args.count,
			.type = args.type
		};
		m_rhi = args.rhi->CreateQueryHeap(desc);
	}

	RHIQueryHeap* GPUQueryHeap::GetRHI() const
	{
		return m_rhi.get();
	}
}