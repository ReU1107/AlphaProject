#include "VulkanCommandQueue.h"

#include "VulkanSwapchain.h"
#include "VulkanCommandBuffer.h"
#if RHI_VK

namespace Alpha
{
	VulkanCommandQueue::VulkanCommandQueue(VulkanDevice* device)
		: RHICommandQueue()
		, VulkanDeviceChild(device)
	{
	}

	VulkanCommandQueue::~VulkanCommandQueue()
	{
		auto device = GetVkLogicalDevice();

		vkDestroyFence(device, mFence, nullptr);

	}

	void VulkanCommandQueue::Initialize(const RHICommandQueueDesc& desc)
	{
		auto device = GetVkLogicalDevice();
		VkQueue queue = VK_NULL_HANDLE;

		// BitFlagsを使用して作成済みのキュー番号から作成しないようにする
		auto index = this->GetGraphicsQueueFamilyIndex();

		vkGetDeviceQueue(device, index, 0, &queue);

		VkFenceCreateInfo fence_info = {};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		VkFence fence = VK_NULL_HANDLE;

		vkCreateFence(device, &fence_info, nullptr, &fence);

		mQueue = queue;
		mFence = fence;
	}

	void VulkanCommandQueue::Begin()
	{
		auto device = GetVkLogicalDevice();

		vkResetFences(device, 1, &mFence);

	}

	void VulkanCommandQueue::SetWaitSemaphore(RHISwapchain* swapchain_)
	{
		auto swapchain = static_cast<VulkanSwapchain*>(swapchain_);

		mCurrentSemaphore = swapchain->GetSignalSemaphore();
	}

	void VulkanCommandQueue::ExecuteCommandBuffer(RHICommandBuffer* command_buffer_)
	{
		auto command_buffer = static_cast<VulkanCommandBuffer*>(command_buffer_);

		VkCommandBuffer process_buffer = command_buffer->GetCurrentCommandBuffer();
		//VkSemaphore signal_semaphore = command_buffer->GetCurrentSemaphore();
		VkPipelineStageFlags wait_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		// 処理するコマンドバッファ
		submit_info.pCommandBuffers = &process_buffer;
		submit_info.commandBufferCount = 1;

		submit_info.pWaitDstStageMask = &wait_stage_mask;
		if (mCurrentSemaphore)
		{
			// このサブミット行う前に終わらせたい処理を待つ
			submit_info.waitSemaphoreCount = 1;
			submit_info.pWaitSemaphores = &mCurrentSemaphore;
		}


		// このサブミットに依存するものはこのシグナルを使用する
		// Todo:いつか復活させる。
		//submit_info.signalSemaphoreCount = 1;
		//submit_info.pSignalSemaphores = &signal_semaphore;
		auto queue = mQueue;
		auto fence = mFence;
		auto result = vkQueueSubmit(queue, 1, &submit_info, fence);
		if(result != VK_SUCCESS)
		{

		}
		//mCurrentSemaphore.push_back(signal_semaphore);
	}

	void VulkanCommandQueue::Wait()
	{
		auto device = GetVkLogicalDevice();
		auto result = vkWaitForFences(device, 1, &mFence, VK_TRUE, UINT64_MAX);
		if (result != VK_SUCCESS)
		{

		}

	}

	void VulkanCommandQueue::Present(RHISwapchain* swapchain_)
	{
		auto swapchain = static_cast<VulkanSwapchain*>(swapchain_);

		VkSwapchainKHR swapchains = swapchain->GetNative();
		auto image_index = swapchain->GetCurrentImageIndex();
		auto queue = mQueue;
		//VkSemaphore semaphores = swapchain->GetWaitSemaphore();

		VkPresentInfoKHR present_info{};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		//present_info.waitSemaphoreCount = 1;
		//present_info.pWaitSemaphores = &semaphores;
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &swapchains;
		present_info.pImageIndices = &image_index;

		auto result = vkQueuePresentKHR(queue, &present_info);
		if (result != VK_SUCCESS)
		{

		}
		//swapchain->End();
	}

}
#endif