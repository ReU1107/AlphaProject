#include "RenderHardwareInterface.h"

#include "RHI/RHIBuffer.h"
#include "RHI/RHITexture.h"
#include "RHI/RHISwapChain.h"
#include "RHI/RHICommandBuffer.h"
#include "RHI/RHICommandQueue.h"
#include "RHI/RHITimeline.h"
#include "RHI/RHISamplerState.h"
#include "RHI/RHIDescriptorHeap.h"
#include "RHI/RHIPipelineLayout.h"
#include "RHI/RHIShader.h"
#include "RHI/RHIGraphicsPipeline.h"
#include "RHI/RHIComputePipeline.h"

#if RHI_D3D12
#include "D3D12/D3D12Dynamic.h"
#elif RHI_VK
#include "Vulkan/VulkanDynamic.h"
#endif

namespace Alpha
{
	RenderHardwareInterface::RenderHardwareInterface() = default;
	RenderHardwareInterface::~RenderHardwareInterface() = default;

	bool RenderHardwareInterface::Initialize(const Args& args)
	{
#if RHI_D3D12
		m_dynamic = std::make_unique<D3D12Dynamic>();
#elif RHI_VK
		m_dynamic = std::make_unique<VulkanDynamic>();
#endif

		return true;
	}

	void RenderHardwareInterface::Finalize()
	{
		m_dynamic.reset();
	}

	TUnique<RHIBuffer> RenderHardwareInterface::CreateBuffer(const RHIBufferDesc& desc)
	{
		return m_dynamic->CreateBuffer(desc);
	}

	TUnique<RHITexture> RenderHardwareInterface::CreateTexture(const RHITextureDesc& desc)
	{
		return m_dynamic->CreateTexture(desc);
	}

	TUnique<RHISwapChain> RenderHardwareInterface::CreateSwapChain(const RHISwapChainDesc& desc)
	{
		return m_dynamic->CreateSwapChain(desc);
	}

	TUnique<RHICommandBuffer> RenderHardwareInterface::CreateCommandBuffer(const RHICommandBufferDesc& desc)
	{
		return m_dynamic->CreateCommandBuffer(desc);
	}

	TUnique<RHICommandQueue> RenderHardwareInterface::CreateCommandQueue(const RHICommandQueueDesc& desc)
	{
		return m_dynamic->CreateCommandQueue(desc);
	}

	TUnique<RHITimeline> RenderHardwareInterface::CreateTimeline(const RHITimelineDesc& desc)
	{
		return m_dynamic->CreateTimeline(desc);
	}

	TUnique<RHISamplerState> RenderHardwareInterface::CreateSamplerState(const RHISamplerStateDesc& desc)
	{
		return m_dynamic->CreateSamplerState(desc);
	}

	TUnique<RHIDescriptorHeap> RenderHardwareInterface::CreateDescriptorHeap(const RHIDescriptorHeapDesc& desc)
	{
		return m_dynamic->CreateDescriptorHeap(desc);
	}

	TUnique<RHIPipelineLayout> RenderHardwareInterface::CreatePipelineLayout(const RHIPipelineLayoutDesc& desc)
	{
		return m_dynamic->CreatePipelineLayout(desc);
	}

	//TUnique<RHIDescriptorView> RenderHardwareInterface::CreateDescriptorView(const RHIDescriptorViewDesc& desc)
	//{
	//	return m_dynamic->CreateDescriptorView(desc);
	//}

	TUnique<RHIShader> RenderHardwareInterface::CreateShader(const RHIShaderDesc& desc)
	{
		return m_dynamic->CreateShader(desc);
	}

	TUnique<RHIGraphicsPipeline> RenderHardwareInterface::CreateGraphicsPipeline(const RHIGraphicsPipelineDesc& desc)
	{
		return m_dynamic->CreateGraphicsPipeline(desc);
	}

	TUnique<RHIComputePipeline> RenderHardwareInterface::CreateComputePipeline(const RHIComputePipelineDesc& desc)
	{
		return m_dynamic->CreateComputePipeline(desc);
	}

	void RenderHardwareInterface::RequestTopLevelASSize(const RHITopLevelASDesc& desc, RHIASMemoryInfo* info)
	{
		m_dynamic->RequestTopLevelASSize(desc, info);
	}

	void RenderHardwareInterface::RequestBottomLevelASSize(const RHIBuildASDesc& desc, RHIASMemoryInfo* info)
	{
		m_dynamic->RequestBottomLevelASSize(desc, info);
	}
}
