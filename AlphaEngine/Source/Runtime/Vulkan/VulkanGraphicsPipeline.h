#pragma once

#include "VulkanDevice.h"
#if RHI_VK

namespace Alpha
{
	class VulkanGraphicsPipeline : public VulkanDeviceChild
	{
	private:

	public:
		VulkanGraphicsPipeline(VulkanDevice* device);
	

		void Create();


	};

}
#endif