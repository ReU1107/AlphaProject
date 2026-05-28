#pragma once
#include "VulkanDevice.h"
#include "RHI/RHISwapchain.h"
#if RHI_VK

#include <vector>
#ifdef _WIN64
#include <Windows.h>
#endif // _WIN64

namespace Alpha
{

	struct VulkanBackBuffer
	{
		VkImage Image{ VK_NULL_HANDLE };				// 
		VkImageView ImageView{ VK_NULL_HANDLE };		// 
		VkSemaphore SignalSemaphore{ VK_NULL_HANDLE };	// Presentを待たせる
		//VkSemaphore WaitSemaphore{ VK_NULL_HANDLE };	// Presentをする時にこのセマフォを持つ処理を待つ為に使用する(配列になる可能性あり)


		VkImageSubresourceRange mSubresourceRange{};
		VkImageLayout CurrentLayout;
		VkImageLayout NextLayout;
	};

	class VulkanSwapchain : public RHISwapchain, public VulkanDeviceChild
	{
	public:
		VulkanSwapchain(VulkanDevice* device);
		virtual ~VulkanSwapchain();

		// 
		VkSwapchainKHR GetNative() const { return mSwapchain; }


		VkImage GetCurrentImage() const { return mBackBuffers.at(mCurrentImageIndex).Image; }
		VkImageView GetCurrentImageView() const { return mBackBuffers.at(mCurrentImageIndex).ImageView; }

		// 
		VkSemaphore GetSignalSemaphore() const;
		// 
		//VkSemaphore GetWaitSemaphore() const;
		// 
		uint32_t GetCurrentImageIndex() const { return mCurrentImageIndex; }

		std::vector<VulkanBackBuffer>& GetBackBuffers() { return mBackBuffers; }
		virtual bool Initialize(const RHISwapchainDesc& desc) override;
		// 
		virtual uint32_t GetCurrentBackBufferIndex() const override;

		virtual void Begin() override;

		virtual void End() override;

		//virtual void WaitQueue(RHICommandQueue* command_queue) override;

		// 
		void CreateSwapchain();

#ifdef _WIN64
		void CreateSurface(HWND hWnd);
#endif // _WIN64

	private:
		VkSwapchainKHR mSwapchain{ VK_NULL_HANDLE };
		VkSurfaceKHR mSurface{ VK_NULL_HANDLE };
		std::vector<VulkanBackBuffer> mBackBuffers;
		uint32_t mCurrentFrameIndex{ 0 };
		uint32_t mMaxFrameCount{ 0 };
		uint32_t mCurrentImageIndex{ 0 };

	};

}

#endif