#pragma once
#include "RHI/RHIShader.h"
#include "VulkanDevice.h"
#if RHI_VK

namespace Alpha
{
	class VulkanShader : public RHIShader, public VulkanDeviceChild
	{
	private:
		VkShaderModule mModule{ VK_NULL_HANDLE };

	public:
		VulkanShader(VulkanDevice* device);
		~VulkanShader();

		virtual void Initialize(const RHIShaderDesc& desc) override;

		VkShaderModule GetModule() const { return mModule; }
	};

}

#endif
