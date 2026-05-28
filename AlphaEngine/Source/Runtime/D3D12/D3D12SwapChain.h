#pragma once
#include "RHI/RHISwapChain.h"

#if RHI_D3D12
#include "D3D12Device.h"
#include <dxgi1_6.h>

namespace Alpha
{
	
	class D3D12SwapChain : public RHISwapChain, public D3D12DeviceChild
	{
	private:
		ComPtr<IDXGISwapChain4> m_swap_chain{ nullptr };
	public:
		D3D12SwapChain(D3D12Device* device);
		virtual ~D3D12SwapChain();

		virtual bool Initialize(const RHISwapChainDesc& desc) final;

		virtual void Present() final;

		virtual UInt32 GetCurrentBackBufferIndex() const final;

		ID3D12Resource1* GetBackBuffer(UInt32 index) const;
	};

}

#endif