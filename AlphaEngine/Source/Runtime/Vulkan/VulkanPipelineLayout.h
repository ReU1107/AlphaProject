#pragma once
#include "VulkanDevice.h"
#if RHI_VK

namespace Alpha
{
	class VulkanPipelineLayout : public VulkanDeviceChild
	{

	public:
		VulkanPipelineLayout(VulkanDevice* device);
		~VulkanPipelineLayout();
	};

}
#endif