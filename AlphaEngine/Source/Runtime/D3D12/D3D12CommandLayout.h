#pragma once
#include "D3D12Device.h"
#include "RHI/RHICommandLayout.h"

#if RHI_D3D12

namespace Alpha
{
	class D3D12CommandLayout : public RHICommandLayout, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12CommandSignature> m_signature{ nullptr };
	public:
		D3D12CommandLayout(D3D12Device* device);
		~D3D12CommandLayout();

		bool Initialize(const RHICommandLayoutDesc& desc) final;

		ID3D12CommandSignature* GetNativeHandle() const { return m_signature.Get(); }


	};
}

#endif