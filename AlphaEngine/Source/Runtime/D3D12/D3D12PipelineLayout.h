#pragma once
#include "D3D12Device.h"
#include "RHI/RHIPipelineLayout.h"
#if RHI_D3D12

namespace Alpha
{
	class D3D12PipelineLayout : public RHIPipelineLayout, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12RootSignature> m_root_signature{ nullptr };

	public:
		D3D12PipelineLayout(D3D12Device* device);
		~D3D12PipelineLayout();

		bool Initialize(const RHIPipelineLayoutDesc& desc) final;

		ID3D12RootSignature* GetNativeHandle() const { return m_root_signature.Get(); }

	};

}

#endif
