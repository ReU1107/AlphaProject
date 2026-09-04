#pragma once

namespace Alpha
{
	class RHIBuffer;
	class RHICommandBuffer;
	class RHICommandQueue;
	class RHICommandLayout;
	class RHIComputePipeline;
	class RHIDescriptorHeap;
	class RHIDescriptorView;
	class RHIGraphicsPipeline;
	class RHIRayTracePipeline;
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
	struct RHICommandLayoutDesc;
	struct RHIComputePipelineDesc;
	struct RHIDescriptorHeapDesc;
	struct RHIDescriptorLayoutDesc;
	struct RHIDescriptorViewDesc;
	struct RHIGraphicsPipelineDesc;
	struct RHIRayTracePipelineDesc;
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
	struct RHIRayTraceDesc;
}