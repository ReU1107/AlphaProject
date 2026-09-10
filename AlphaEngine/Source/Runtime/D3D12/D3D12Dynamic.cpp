#include "D3D12Dynamic.h"
#include "D3D12Device.h"

#include "D3D12Buffer.h"
#include "D3D12CommandBuffer.h"
#include "D3D12CommandQueue.h"
#include "D3D12CommandLayout.h"
#include "D3D12ComputePipeline.h"
#include "D3D12DescriptorHeap.h"
#include "D3D12GraphicsPipeline.h"
#include "D3D12PipelineLayout.h"
#include "D3D12RayTracePipeline.h"
#include "D3D12SamplerState.h"
#include "D3D12Shader.h"
#include "D3D12SwapChain.h"
#include "D3D12Texture.h"
#include "D3D12Timeline.h"

#if RHI_D3D12

namespace Alpha
{

    D3D12Dynamic::D3D12Dynamic()
        : Base()
    {
        m_device = std::make_unique<D3D12Device>();
        m_device->Initialize();
    }

    D3D12Dynamic::~D3D12Dynamic() = default;

    TUnique<RHISwapChain> D3D12Dynamic::CreateSwapChain(const RHISwapChainDesc& desc)
    {
        return CreateRHIObject<D3D12SwapChain>(m_device.get(), desc);
    }

    TUnique<RHIBuffer> D3D12Dynamic::CreateBuffer(const RHIBufferDesc& desc)
    {
        return CreateRHIObject<D3D12Buffer>(m_device.get(), desc);
    }

    TUnique<RHITexture> D3D12Dynamic::CreateTexture(const RHITextureDesc& desc)
    {
        return CreateRHIObject<D3D12Texture>(m_device.get(), desc);
    }

    TUnique<RHISamplerState> D3D12Dynamic::CreateSamplerState(const RHISamplerStateDesc& desc)
    {
        return CreateRHIObject<D3D12SamplerState>(m_device.get(), desc);
    }

    TUnique<RHICommandBuffer> D3D12Dynamic::CreateCommandBuffer(const RHICommandBufferDesc& desc)
    {
        return CreateRHIObject<D3D12CommandBuffer>(m_device.get(), desc);
    }

    TUnique<RHICommandQueue> D3D12Dynamic::CreateCommandQueue(const RHICommandQueueDesc& desc)
    {
        return CreateRHIObject<D3D12CommandQueue>(m_device.get(), desc);
    }

    TUnique<RHICommandLayout> D3D12Dynamic::CreateCommandLayout(const RHICommandLayoutDesc& desc)
    {
        return CreateRHIObject<D3D12CommandLayout>(m_device.get(), desc);
    }

    TUnique<RHITimeline> D3D12Dynamic::CreateTimeline(const RHITimelineDesc& desc)
    {
        return CreateRHIObject<D3D12Timeline>(m_device.get(), desc);
    }

    TUnique<RHIShader> D3D12Dynamic::CreateShader(const RHIShaderDesc& desc)
    {
        return CreateRHIObject<D3D12Shader>(m_device.get(), desc);
    }

    TUnique<RHIDescriptorHeap> D3D12Dynamic::CreateDescriptorHeap(const RHIDescriptorHeapDesc& desc)
    {
        return CreateRHIObject<D3D12DescriptorHeap>(m_device.get(), desc);
    }

    TUnique<RHIPipelineLayout> D3D12Dynamic::CreatePipelineLayout(const RHIPipelineLayoutDesc& desc)
    {
        return CreateRHIObject<D3D12PipelineLayout>(m_device.get(), desc);
    }

    //TUnique<RHIDescriptorView> D3D12Dynamic::CreateDescriptorView(const RHIDescriptorViewDesc& desc)
    //{
    //    return nullptr;
    //}

    TUnique<RHIGraphicsPipeline> D3D12Dynamic::CreateGraphicsPipeline(const RHIGraphicsPipelineDesc& desc)
    {
        return CreateRHIObject<D3D12GraphicsPipeline>(m_device.get(), desc);
    }

    TUnique<RHIComputePipeline> D3D12Dynamic::CreateComputePipeline(const RHIComputePipelineDesc& desc)
    {
        return CreateRHIObject<D3D12ComputePipeline>(m_device.get(), desc);
    }

    TUnique<RHIRayTracePipeline> D3D12Dynamic::CreateRayTracePipeline(const RHIRayTracePipelineDesc& desc)
    {
        return CreateRHIObject<D3D12RayTracePipeline>(m_device.get(), desc);
    }
    
    void D3D12Dynamic::RequestTLASSize(const RHIBuildTLASDesc& desc, RHIASMemoryInfo* info)
    {
        m_device->RequestTopLevelASSize(desc, info);
    }

    void D3D12Dynamic::RequestBLASSize(const RHIBuildBLASDesc& desc, RHIASMemoryInfo* info)
    {
        m_device->RequestBottomLevelASSize(desc, info);
    }
}
#endif