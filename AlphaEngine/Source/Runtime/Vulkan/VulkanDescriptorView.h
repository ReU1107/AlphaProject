#pragma once
#include "RHI/RHIDescriptorView.h"
#include "VulkanDevice.h"
#include <vector>
#include <bitset>
#if RHI_VK

namespace Alpha
{
	class RHIBuffer;


	class VulkanDescriptorView : public RHIDescriptorView, public VulkanDeviceChild
	{
	private:
		std::vector<VkDescriptorSet> mSets{};
		VkPipelineLayout mPipelineLayout{ VK_NULL_HANDLE };
		std::bitset<8>	mDynamicSets{};						// まぁ一つしかtrueにならないんですけど

	public:
		VulkanDescriptorView(VulkanDevice* device);
		~VulkanDescriptorView();

		VkDescriptorSet GetNativeHandle(uint32_t index) const { return mSets[index]; }

		VkPipelineLayout GetNativeLayout() const { return mPipelineLayout; }

		const std::vector<VkDescriptorSet>& GetNativeHandles() const { return mSets; }

		virtual void Initialize(const RHIDescriptorViewDesc& desc) override;

		void BindResource(RHIBuffer* buffer, uint32_t set_index, uint32_t register_index);

	};

}
#endif