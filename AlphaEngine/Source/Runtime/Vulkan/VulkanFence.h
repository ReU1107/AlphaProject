#pragma once
#include "VulkanDevice.h"
#include "RHI/RHITimeline.h"

#if RHI_VK

namespace Alpha
{
	class VulkanFence : public RHITimeline, public VulkanDeviceChild
	{
	private:
		VkFence mFence{ VK_NULL_HANDLE };


	public:
		VulkanFence(VulkanDevice* device);
		~VulkanFence();

		virtual bool Initialize(const RHIFenceDesc& desc) override final;

	};
}

#endif