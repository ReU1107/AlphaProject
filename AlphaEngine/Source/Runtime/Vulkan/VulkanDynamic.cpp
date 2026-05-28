#include "VulkanDynamic.h"
#include <cstdint>

#if RHI_VK

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#pragma comment(lib,"vulkan-1.lib")

#include "RHI/RHIDefinitions.h"

#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanBuffer.h"
#include "VulkanCommandBuffer.h"
#include "VulkanCommandQueue.h"
#include "VulkanShader.h"
#include "VulkanDescriptorHeap.h"
#include "VulkanDescriptorLayout.h"
#include "VulkanDescriptorView.h"



namespace Alpha
{
	VulkanDynamic::VulkanDynamic()
	{
		mDevice = new VulkanDevice();

		SetInstance(this);

		//vk::CommandBuffer c;
		//c.pipelineBarrier(, , , nullptr, nullptr,, nullptr);

	}

	VulkanDynamic::~VulkanDynamic()
	{
		delete mDevice;
	}
	
	void VulkanDynamic::Init()
	{
	}

	RHISwapchain* VulkanDynamic::CreateSwapchain(RHISwapchainDesc& desc)
	{
		VulkanSwapchain* swapchain = new VulkanSwapchain(mDevice);
		// Windows依存
		swapchain->CreateSurface(desc.hWnd);

		swapchain->CreateSwapchain();
		return swapchain;
	}
	
	RHIBuffer* VulkanDynamic::CreateBuffer(RHIBufferDesc& desc)
	{
		VulkanBuffer* buffer = new VulkanBuffer(mDevice);
		buffer->Initialize(desc);
		return buffer;
	}

	RHITexture* VulkanDynamic::CreateTexture(RHITextureDesc& desc)
	{
		return nullptr;
	}
	
	RHICommandBuffer* VulkanDynamic::CreateCommandBuffer(RHICommandBufferDesc& desc)
	{
		VulkanCommandBuffer::InitializeExtension(mDevice);
		return CreateRHIObject<VulkanCommandBuffer>(mDevice, desc);
	}

	RHICommandQueue* VulkanDynamic::CreateCommandQueue(RHICommandQueueDesc& desc)
	{
		return CreateRHIObject<VulkanCommandQueue>(mDevice, desc);
	}

	RHIShader* VulkanDynamic::CreateShader(RHIShaderDesc& desc)
	{
		return CreateRHIObject<VulkanShader>(mDevice, desc);
	}

	RHIDescriptorHeap* VulkanDynamic::CreateDescriptorHeap(RHIDescriptorHeapDesc& desc)
	{
		return CreateRHIObject<VulkanDescriptorHeap>(mDevice, desc);
	}

	RHIDescriptorLayout* VulkanDynamic::CreateDescriptorLayout(RHIDescriptorLayoutDesc& desc)
	{
		return CreateRHIObject<VulkanDescriptorLayout>(mDevice, desc);
	}

	RHIDescriptorView* VulkanDynamic::CreateDescriptorView(RHIDescriptorViewDesc& desc)
	{
		return CreateRHIObject<VulkanDescriptorView>(mDevice, desc);
	}
}

#endif