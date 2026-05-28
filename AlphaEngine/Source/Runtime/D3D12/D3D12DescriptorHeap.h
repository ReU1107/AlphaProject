#pragma once
#include "D3D12Device.h"
#include "RHI/RHIDescriptorHeap.h"

#if RHI_D3D12

namespace Alpha
{
	class D3D12DescriptorHeap : public RHIDescriptorHeap, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12DescriptorHeap> m_heap{ nullptr };
	public:
		D3D12DescriptorHeap(D3D12Device* device);
		~D3D12DescriptorHeap();

		bool Initialize(const RHIDescriptorHeapDesc& desc) final;

		bool CreateView(const RHIResourceViewDesc& desc) final;

		ID3D12DescriptorHeap* GetNativeHandle() const { return m_heap.Get(); }

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UInt32 index) const;

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UInt32 index) const;
	};

}

#endif