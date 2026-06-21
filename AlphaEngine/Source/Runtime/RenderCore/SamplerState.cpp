#include "SamplerState.h"
#include "DescriptorHeap.h"
#include "RenderHardwareInterface.h"
#include "RHI/RHISamplerState.h"

namespace Alpha
{
	SamplerState::SamplerState() = default;
	SamplerState::~SamplerState()
	{
		if(m_handle.IsActive())
		{
			m_handle.Free();
		}
	}

	bool SamplerState::Initialize(const Args& args)
	{
		m_handle = args.sampler_heap->Allocate(ERHIResourceViewType::SamplerView);

		RHISamplerStateDesc desc = {};
		desc.descriptor_heap = m_handle.heap->GetRHI();
		desc.heap_index = m_handle.index;
        desc.filter = args.filter;
        desc.address_u = args.address_u;
        desc.address_v = args.address_v;
        desc.address_w = args.address_w;

		m_rhi = args.rhi->CreateSamplerState(desc);

		return true;
	}

	void SamplerState::Finalize()
	{
	}
}