#pragma once
#include "RHI/RHIPipelineLayout.h"
#include "VulkanDevice.h"
#include <vector>
#if RHI_VK

namespace Alpha
{

	class VulkanDescriptorLayout : public RHIDescriptorLayout, public VulkanDeviceChild
	{
	private:
		VkDescriptorSetLayout mDescriptorLayout{ VK_NULL_HANDLE };
		bool mIsDynamimc{ false };
	public:
		VulkanDescriptorLayout(VulkanDevice* device);
		~VulkanDescriptorLayout();

		VkDescriptorSetLayout GetNativeHandle() const { return mDescriptorLayout; }

		virtual void Initialize(const RHIDescriptorLayoutDesc& desc) override;


		bool IsDynamic() const { return mIsDynamimc; }
	};

}
#endif