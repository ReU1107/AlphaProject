#pragma once
#include "D3D12Device.h"
#include "RHI/RHIGraphicsPipeline.h"

#if RHI_D3D12

namespace Alpha
{
	class D3D12GraphicsPipeline : public RHIGraphicsPipeline, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12PipelineState> m_pipeline_state{ nullptr };

	public:
		D3D12GraphicsPipeline(D3D12Device* device);
		~D3D12GraphicsPipeline();

		bool Initialize(const RHIGraphicsPipelineDesc& desc) final;


		ID3D12PipelineState* GetNativeHandle() const { return m_pipeline_state.Get(); }

	};
}
#endif