#include "VulkanDescriptorHeap.h"
#include "RHI/RHIDefinitions.h"
#include "VulkanPrivate.h"
#include <vector>
#if RHI_VK

namespace Alpha
{
	VulkanDescriptorHeap::VulkanDescriptorHeap(VulkanDevice* device)
		: RHIDescriptorHeap()
		, VulkanDeviceChild(device)
	{

	}

	VulkanDescriptorHeap::~VulkanDescriptorHeap()
	{
		auto device = GetVkLogicalDevice();

		vkDestroyDescriptorPool(device, mPool, nullptr);
	}


	void VulkanDescriptorHeap::Initialize(const RHIDescriptorHeapDesc& desc)
	{
		auto device = GetVkLogicalDevice();

		std::vector<VkDescriptorPoolSize> parameters(desc.ParameterCount);

		for (uint32_t i = 0; i < desc.ParameterCount; i++)
		{
			auto& parameter = parameters[i];
			parameter.descriptorCount = desc.Parameters[i].ReservationCount;
			parameter.type = GetVkDescriptorType(desc.Parameters[i].ViewType);
		}

		VkDescriptorPoolCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		info.pPoolSizes = parameters.data();
		info.poolSizeCount = parameters.size();
		info.flags;//VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT ,VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT
		// ↑のパラメータをこの数分確保する
		info.maxSets = 8;

		VkDescriptorPool pool = VK_NULL_HANDLE;
		auto result = vkCreateDescriptorPool(device, &info, nullptr, &pool);
		if (result != VK_SUCCESS)
		{

		}

	}
}
#endif