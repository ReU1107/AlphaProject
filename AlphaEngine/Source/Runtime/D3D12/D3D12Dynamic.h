#pragma once
#include "RHI/RHIDynamic.h"

#if RHI_D3D12

namespace Alpha
{
	class D3D12Device;

	class D3D12Dynamic : public RHIDynamic
	{
	private:
		using Base = RHIDynamic;

	public:
		D3D12Dynamic();
		virtual ~D3D12Dynamic();

        TUnique<RHISwapChain> CreateSwapChain(const RHISwapChainDesc& desc) final;

        TUnique<RHIBuffer> CreateBuffer(const RHIBufferDesc& desc) final;

        TUnique<RHITexture> CreateTexture(const RHITextureDesc& desc) final;

        TUnique<RHISamplerState> CreateSamplerState(const RHISamplerStateDesc& desc) final;

        TUnique<RHICommandBuffer> CreateCommandBuffer(const RHICommandBufferDesc& desc) final;

        TUnique<RHICommandQueue> CreateCommandQueue(const RHICommandQueueDesc& desc) final;

        TUnique<RHICommandSignature> CreateCommandSignature(const RHICommandSignatureDesc& desc) final;

        TUnique<RHITimeline> CreateTimeline(const RHITimelineDesc& desc) final;

        TUnique<RHIShader> CreateShader(const RHIShaderDesc& desc) final;

        TUnique<RHIDescriptorHeap> CreateDescriptorHeap(const RHIDescriptorHeapDesc& desc) final;

        TUnique<RHIPipelineLayout> CreatePipelineLayout(const RHIPipelineLayoutDesc& desc) final;

        //TUnique<RHIDescriptorView> CreateDescriptorView(const RHIDescriptorViewDesc& desc) final;

        TUnique<RHIGraphicsPipeline> CreateGraphicsPipeline(const RHIGraphicsPipelineDesc& desc) final;

        TUnique<RHIComputePipeline> CreateComputePipeline(const RHIComputePipelineDesc& desc) final;


	public:
		TUnique<D3D12Device> m_device{ nullptr };

		D3D12Device* GetDevice() { return m_device.get(); }
	};


}

#endif