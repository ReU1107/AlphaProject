#include "VulkanPipelineLayout.h"
#include <vector>
#if RHI_VK

namespace Alpha
{
	VulkanPipelineLayout::VulkanPipelineLayout(VulkanDevice* device)
		: VulkanDeviceChild(device)
	{
	}

	VulkanPipelineLayout::~VulkanPipelineLayout()
	{
		//std::vector<VkDescriptorSetLayout> set_layouts;
		//for (uint32_t i = 0; i < ;;)
		//{
		//	set_layouts[i] = 0;
		//}


		//VkPipelineLayoutCreateInfo info = {};
		//info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		//info.pSetLayouts = set_layouts.data();
		//info.setLayoutCount = set_layouts.size();
		//info.flags;

		//auto device = GetVkLogicalDevice();
		//VkPipelineLayout layout = VK_NULL_HANDLE;
		//auto result = vkCreatePipelineLayout(device, &info, nullptr, &layout);
		//if (result != VK_SUCCESS)
		//{

		//}
	}
}
#endif