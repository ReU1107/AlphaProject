#include "VulkanShaderLink.h"

#if RHI_VK

namespace Alpha
{
	VulkanShaderLink::VulkanShaderLink(VulkanDevice* device)
		: VulkanDeviceChild(device)
	{
	}

	VulkanShaderLink::~VulkanShaderLink()
	{
	}

	static bool IsExtensionInitialized = false;

	void VulkanShaderLink::InitializeExtension(VulkanDevice* device_)
	{
		// 既に初期化済み
		if (IsExtensionInitialized)
			return;
		PFN_vkCreateShadersEXT;

		auto device = device_->GetVkDevice();
		//vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR"));
		//vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(device, "vkCmdEndRenderingKHR"));


		IsExtensionInitialized = true;
	}
}
#endif