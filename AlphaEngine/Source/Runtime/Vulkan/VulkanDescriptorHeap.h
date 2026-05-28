#pragma once
#include "RHI/RHIDescriptorHeap.h"
#include "VulkanDevice.h"
#if RHI_VK

namespace Alpha
{
	class VulkanDescriptorHeap : public RHIDescriptorHeap, public VulkanDeviceChild
	{
	private:
		VkDescriptorPool mPool{ VK_NULL_HANDLE };

	public:
		VulkanDescriptorHeap(VulkanDevice* device);
		~VulkanDescriptorHeap();

		VkDescriptorPool GetNativeHandle() const { return mPool; }

		virtual void Initialize(const RHIDescriptorHeapDesc& desc) override;


	};


}
#endif