#include "VulkanDescriptorLayout.h"
#include "RHI/RHIDefinitions.h"

#include "VulkanPrivate.h"
#if RHI_VK


namespace Alpha
{
	VulkanDescriptorLayout::VulkanDescriptorLayout(VulkanDevice* device)
		: VulkanDeviceChild(device)
	{
	}

	VulkanDescriptorLayout::~VulkanDescriptorLayout()
	{
		VkWriteDescriptorSet;

	}

	void VulkanDescriptorLayout::Initialize(const RHIDescriptorLayoutDesc& desc)
	{

		/*
		* layout(set = 〇,binding = 〇)
		* set = これが設定下セットの番号
		* 一つのセットしか使用しなけらばすべての番号が0になる
		セットの数=DescriptorSetLayoutの数になる
		*/

		std::vector<VkDescriptorSetLayoutBinding> bindings(desc.ParameterCount);

		for (uint32_t i = 0; i < desc.ParameterCount; i++)
		{
			const auto& parameter = desc.Parameters[i];
			auto& binding = bindings[i];
			// glsl layout(binding)の番号は(hlsl register())
			binding.binding = parameter.RegisterIndex;
			// バインドするリソースのタイプは
			binding.descriptorType = GetVkDescriptorType(parameter.ViewType);
			// 配列の個数は
			binding.descriptorCount = parameter.ResourceCount;
			// 使用するシェーダーは
			binding.stageFlags = GetVkShaderStage(parameter.ShaderFlags);

		}


		VkDescriptorSetLayoutCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

		info.pBindings = bindings.data();
		info.bindingCount = bindings.size();
		info.flags |= desc.IsDynamic ? VkDescriptorSetLayoutCreateFlagBits::VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : 0;
		auto device = GetVkLogicalDevice();

		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
		vkCreateDescriptorSetLayout(device, &info, nullptr, &layout);
		mDescriptorLayout = layout;
		mIsDynamimc = desc.IsDynamic;
	}


}
#endif