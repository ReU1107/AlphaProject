#include "VulkanShader.h"
#include "RHI/RHIDefinitions.h"

#include <fstream>
#include <vector>

#if RHI_VK

namespace Alpha
{
	VulkanShader::VulkanShader(VulkanDevice* device)
		: RHIShader()
		, VulkanDeviceChild(device)
	{
	}

	VulkanShader::~VulkanShader()
	{

		auto device = GetVkLogicalDevice();

		vkDestroyShaderModule(device, mModule, nullptr);

	}

	void VulkanShader::Initialize(const RHIShaderDesc& desc)
	{
		auto device = GetVkLogicalDevice();

		String file_path(desc.file_directory);
		std::fstream file(file_path.c_str(), std::ios::in | std::ios::binary);
		if (!file.good()) abort();
		std::vector< std::uint8_t > code;
		code.assign(
			std::istreambuf_iterator< char >(file),
			std::istreambuf_iterator< char >()
		);
		// シェーダモジュールを作る
		VkShaderModuleCreateInfo shader_module_info = {};
		shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shader_module_info.pNext = nullptr;
		shader_module_info.flags = 0u;
		shader_module_info.codeSize = code.size();

		VkShaderModule shader_module = VK_NULL_HANDLE;
		auto result = vkCreateShaderModule(device, &shader_module_info, nullptr, &shader_module);
		if (result != VK_SUCCESS)
		{

		}
		mModule = shader_module;
	}
}
#endif