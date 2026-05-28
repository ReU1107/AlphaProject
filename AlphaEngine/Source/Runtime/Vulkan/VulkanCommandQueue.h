#pragma once
#include "VulkanDevice.h"
#include "RHI/RHICommandQueue.h"

#include <vector>
#if RHI_VK

namespace Alpha
{
	class VulkanCommandQueue : public RHICommandQueue, public VulkanDeviceChild
	{
	private:
		VkQueue mQueue{ VK_NULL_HANDLE };
		VkFence mFence{ VK_NULL_HANDLE };
		//std::vector<VkSemaphore> mCurrentSemaphores{};
		VkSemaphore mCurrentSemaphore{ VK_NULL_HANDLE };
	public:
		VulkanCommandQueue(VulkanDevice* device);
		~VulkanCommandQueue();

		virtual void Initialize(const RHICommandQueueDesc& desc) override;
		// 
		virtual void Begin() override;
		// 
		virtual void SetWaitSemaphore(RHISwapchain* swapchain_) override;
		// いつかこっちに変える
		//virtual void SetWaitSemaphore(RHISemaphore* semaphore_);

		virtual void ExecuteCommandBuffer(RHICommandBuffer* command_buffer_) override;

		virtual void Wait() override;

		virtual void Present(RHISwapchain* swapchain) override;

	};

}
#endif