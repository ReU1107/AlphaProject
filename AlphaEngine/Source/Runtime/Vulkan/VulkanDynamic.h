#pragma once
#include "RHI/RHIDynamic.h"
#if RHI_VK
namespace Alpha
{
	class VulkanDevice;

	class VulkanDynamic : public RHIDynamic
	{
	private:
		using Base = RHIDynamic;

	public:
		VulkanDynamic();
		virtual ~VulkanDynamic();

		virtual void Init() override;

		virtual RHISwapchain* CreateSwapchain(RHISwapchainDesc& desc) override;

		virtual RHIBuffer* CreateBuffer(RHIBufferDesc& desc) override;

		virtual RHITexture* CreateTexture(RHITextureDesc& desc) override;

		virtual RHICommandBuffer* CreateCommandBuffer(RHICommandBufferDesc& desc) override;

		virtual RHICommandQueue* CreateCommandQueue(RHICommandQueueDesc& desc) override;

		virtual RHIShader* CreateShader(RHIShaderDesc& desc) override;

		virtual RHIDescriptorHeap* CreateDescriptorHeap(RHIDescriptorHeapDesc& desc) override;

		virtual RHIDescriptorLayout* CreateDescriptorLayout(RHIDescriptorLayoutDesc& desc) override;

		virtual RHIDescriptorView* CreateDescriptorView(RHIDescriptorViewDesc& desc) override;

	private:
		VulkanDevice* mDevice;

		VulkanDevice* GetDevice() { return mDevice; }

	};

}
#endif