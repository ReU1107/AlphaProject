#pragma once
#include "D3D12Device.h"
#include "RHI/RHICommandSignature.h"

#if RHI_D3D12

namespace Alpha
{
	class D3D12CommandSignature : public RHICommandSignature, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12CommandSignature> m_signature{ nullptr };
	public:
		D3D12CommandSignature(D3D12Device* device);
		~D3D12CommandSignature();

		bool Initialize(const RHICommandSignatureDesc& desc) final;

	};
}

#endif