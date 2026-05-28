#pragma once
#include "D3D12Device.h"
#include "RHI/RHIWorkGraphPipeline.h"

#if RHI_D3D12

namespace Alpha
{
	class D3D12WorkGraphPipeline : public RHIWorkGraphPipeline, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12PipelineState> m_pipeline_state{ nullptr };

	public:
		D3D12WorkGraphPipeline(D3D12Device* device);
		~D3D12WorkGraphPipeline();

		bool Initialize(const RHIWorkGraphPipelineDesc& desc) final;


		ID3D12PipelineState* GetNativeHandle() const { return m_pipeline_state.Get(); }

	};
}
#endif
