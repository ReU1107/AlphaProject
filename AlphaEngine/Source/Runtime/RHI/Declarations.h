#pragma once

namespace Alpha
{
	class RHIBuffer;
	class RHICommandBuffer;
	class RHICommandQueue;
	class RHICommandSignature;
	class RHIComputePipeline;
	class RHIDescriptorHeap;
	class RHIDescriptorView;
	class RHIGraphicsPipeline;
	class RHIPipelineLayout;
	class RHIResource;
	class RHISamplerState;
	class RHIShader;
	class RHISwapChain;
	class RHITexture;
	class RHITimeline;

	// RHIオブジェクトの生成に必要な情報をまとめた構造体群

	struct RHIBufferDesc;
	struct RHICommandBufferDesc;
	struct RHICommandQueueDesc;
	struct RHICommandSignatureDesc;
	struct RHIComputePipelineDesc;
	struct RHIDescriptorHeapDesc;
	struct RHIDescriptorLayoutDesc;
	struct RHIDescriptorViewDesc;
	struct RHIGraphicsPipelineDesc;
	struct RHIWorkGraphPipelineDesc;
	struct RHIPipelineLayoutDesc;
	struct RHIResourceViewDesc;
	struct RHISamplerStateDesc;
	struct RHIShaderDesc;
	struct RHISwapChainDesc;
	struct RHITextureDesc;
	struct RHITimelineDesc;

	struct RHIBuildBLASDesc;
	struct RHIBuildTLASDesc;
	struct RHIASMemoryInfo;

}