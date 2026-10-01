#pragma once
#include "D3D12Device.h"
#include "RHI/RHIQueryHeap.h"

#if RHI_D3D12

namespace Alpha
{
	class D3D12QueryHeap : public RHIQueryHeap, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12QueryHeap> m_heap{ nullptr };
	public:
		D3D12QueryHeap(D3D12Device* device);
		~D3D12QueryHeap();

		bool Initialize(const RHIQueryHeapDesc& desc) final;


		ID3D12QueryHeap* GetNativeHandle() const { return m_heap.Get(); }

	};

}

#endif
