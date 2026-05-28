#pragma once
#include "VulkanDevice.h"

#if RHI_VK

namespace Alpha
{
	class VulkanShaderLink : public VulkanDeviceChild
	{
	private:
		VkShaderEXT mShaderEXT{ VK_NULL_HANDLE };
	public:
		VulkanShaderLink(VulkanDevice* device);
		~VulkanShaderLink();


		static void InitializeExtension(VulkanDevice* device_);

	};

}
#endif