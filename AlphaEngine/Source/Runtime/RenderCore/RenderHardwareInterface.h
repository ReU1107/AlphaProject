#pragma once
#include "Core/SmartPtr.h"
#include "RHI/Definitions.h"

namespace Alpha
{
	class RHIDynamic;

	/*
	* Render Hardware Interface　
	* 低レベルグラフィックスAPIの窓口
	*/
	class RenderHardwareInterface
	{
	private:
		TUnique<RHIDynamic> m_dynamic{ nullptr };

	public:
		RenderHardwareInterface();
		~RenderHardwareInterface();

		struct Args
		{
			// 追加の初期化パラメーターをここに記述できます
		};

		bool Initialize(const Args& args);
		void Finalize();

		TUnique<RHIBuffer> CreateBuffer(const RHIBufferDesc& desc);

		TUnique<RHITexture> CreateTexture(const RHITextureDesc& desc);

		TUnique<RHISwapChain> CreateSwapChain(const RHISwapChainDesc& desc);

		TUnique<RHICommandBuffer> CreateCommandBuffer(const RHICommandBufferDesc& desc);

		TUnique<RHICommandQueue> CreateCommandQueue(const RHICommandQueueDesc& desc);

		TUnique<RHITimeline> CreateTimeline(const RHITimelineDesc& desc);

		TUnique<RHISamplerState> CreateSamplerState(const RHISamplerStateDesc& desc);

		TUnique<RHIDescriptorHeap> CreateDescriptorHeap(const RHIDescriptorHeapDesc& desc);

		TUnique<RHIPipelineLayout> CreatePipelineLayout(const RHIPipelineLayoutDesc& desc);

		// Vulan用で作ってたけどd3d12側で使わないから保留
		//TUnique<RHIDescriptorView> CreateDescriptorView(const RHIDescriptorViewDesc& desc);

		TUnique<RHIShader> CreateShader(const RHIShaderDesc& desc);

		TUnique<RHIGraphicsPipeline> CreateGraphicsPipeline(const RHIGraphicsPipelineDesc& desc);

		TUnique<RHIComputePipeline> CreateComputePipeline(const RHIComputePipelineDesc& desc);

		TUnique<RHIRayTracePipeline> CreateRayTracePipeline(const RHIRayTracePipelineDesc& desc);


		void RequestTopLevelASSize(const RHIBuildTLASDesc& desc, RHIASMemoryInfo* info);

		void RequestBottomLevelASSize(const RHIBuildBLASDesc& desc, RHIASMemoryInfo* info);

		// テストコード用で外部に公開
		RHIDynamic* GetDynamic() const { return m_dynamic.get(); }

	};

	using RHI = RenderHardwareInterface;

}
