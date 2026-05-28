#pragma once
#include "D3D12Device.h"
#include "RHI/RHITimeline.h"
#if RHI_D3D12

namespace Alpha
{
	class D3D12Timeline : public RHITimeline, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12Fence1> m_fence{ nullptr };
		void* m_event{ nullptr };
	public:
		D3D12Timeline(D3D12Device* device);
		~D3D12Timeline();

		bool Initialize(const RHITimelineDesc& desc) final;

		ID3D12Fence1* GetNativeHandle() const { return m_fence.Get(); }

		void WaitCPU(uint64_t wait_value) final;

	};

}
#endif