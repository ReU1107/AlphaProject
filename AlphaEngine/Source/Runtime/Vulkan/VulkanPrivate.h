#pragma once
#if RHI_VK

#include <vulkan/vulkan.h>
#include "RHI/RHITypes.h"

namespace Alpha
{
	static VkShaderStageFlags GetVkShaderStage(ERHIShaderFlags flags_)
	{
		VkShaderStageFlags flags = 0;

		if (EnumHasFlag(flags_, ERHIShaderFlags::Vertex))
		{
			flags |= VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
		}
		if (EnumHasFlag(flags_, ERHIShaderFlags::Pixel))
		{
			flags |= VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		if (EnumHasFlag(flags_, ERHIShaderFlags::Geometry))
		{
			flags |= VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
		}
		//if (EnumHasFlag(type, ERHIShaderFlags::Domain))
		//{
		//	flags |= VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		//}
		//if (EnumHasFlag(type, ERHIShaderFlags::Hull))
		//{
		//	flags |= VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		//}

		//if (EnumHasFlag(type, ERHIShaderFlags::Mesh))
		//{
		//	flags |= VkShaderStageFlagBits::VK_SHADER_STAGE_MESH_BIT_EXT;
		//}

		return flags;
	}

	static VkDescriptorType GetVkDescriptorType(ERHIResourceViewType type_)
	{
		switch (type_)
		{
		case Alpha::ERHIResourceViewType::UniformBuffer:		return VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case Alpha::ERHIResourceViewType::Texture:				return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		case Alpha::ERHIResourceViewType::Sampler:				return VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLER;
		}

	}

}
#endif