#include "VulkanSwapchain.h"
#include <Windows.h>
#include <vector>
#include "Utility/Utility.h"

#include "VulkanCommandQueue.h"
#if RHI_VK
#include <vulkan/vulkan_win32.h>


namespace Alpha
{
	VulkanSwapchain::VulkanSwapchain(VulkanDevice* device)
		: RHISwapchain()
		, VulkanDeviceChild(device)
	{

	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		auto instance = GetVkInstance();
		auto device = GetVkLogicalDevice();

		for (auto& back_buffer : mBackBuffers)
		{
			vkDestroySemaphore(device, back_buffer.SignalSemaphore, nullptr);

			vkDestroyImageView(device, back_buffer.ImageView, nullptr);

			vkDestroyImage(device, back_buffer.Image, nullptr);
		}
		vkDestroySurfaceKHR(instance, mSurface, nullptr);

		vkDestroySwapchainKHR(device, mSwapchain, nullptr);
	}

	VkSemaphore VulkanSwapchain::GetSignalSemaphore() const
	{
		return mBackBuffers.at(mCurrentFrameIndex).SignalSemaphore;
	}

	//VkSemaphore VulkanSwapchain::GetWaitSemaphore() const
	//{
	//	return mBackBuffers.at(mCurrentFrameIndex).WaitSemaphore;
	//}

	bool VulkanSwapchain::Initialize(const RHISwapchainDesc& desc)
	{
		return false;
	}

	uint32_t VulkanSwapchain::GetCurrentBackBufferIndex() const
	{
		return mCurrentFrameIndex;
	}

	void VulkanSwapchain::Begin()
	{
		auto device = GetVkLogicalDevice();
		auto swapchain = mSwapchain;
		auto semaphore = mBackBuffers.at(mCurrentFrameIndex).SignalSemaphore;
		uint32_t image_index = 0;
		auto result = vkAcquireNextImageKHR(device, swapchain, 0, semaphore, VK_NULL_HANDLE, &image_index);
		if (result != VK_SUCCESS)
		{

		}
		//auto result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &image_index);

		mCurrentImageIndex = image_index;
	}

	void VulkanSwapchain::End()
	{
		SwapIndex(mCurrentFrameIndex, mMaxFrameCount);
	}

	//void VulkanSwapchain::WaitQueue(RHICommandQueue* command_queue_)
	//{
	//	VulkanCommandQueue* command_queue = static_cast<VulkanCommandQueue*>(command_queue_);
	//	//command_queue->

	//	// こいつの処理が終わるのを待つ
	//}

	void VulkanSwapchain::CreateSwapchain()
	{
		auto logical_device = GetVkLogicalDevice();
		auto physical_device = GetVkPhysicalDevice();

		auto surface = mSurface;
		// サーフェイス情報取得
		VkSurfaceCapabilitiesKHR capabilities = {};
		auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &capabilities);
		if (result != VK_SUCCESS)
		{

		}
		uint32_t present_mode_count = 0;
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
		std::vector<VkPresentModeKHR> present_modes(present_mode_count);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, present_modes.data());

		VkPhysicalDeviceFeatures features = {};
		vkGetPhysicalDeviceFeatures(physical_device, &features);
		// サポートしているフォーマット取得
		uint32_t format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
		std::vector<VkSurfaceFormatKHR> formats(format_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, formats.data());


		uint32_t swapchain_count = capabilities.minImageCount;
		// 情報に基づいたスワップチェイン作成
		VkSwapchainCreateInfoKHR swapchain_info = {};
		swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_info.surface = surface;

		swapchain_info.imageExtent = capabilities.currentExtent;
		swapchain_info.imageArrayLayers = capabilities.maxImageArrayLayers;
		swapchain_info.imageFormat = formats[0].format;
		swapchain_info.imageColorSpace = formats[0].colorSpace;
		swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

		swapchain_info.minImageCount = swapchain_count;

		swapchain_info.presentMode = present_modes[0];
		swapchain_info.preTransform = capabilities.currentTransform;

		swapchain_info.clipped = true;
		swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		result = vkCreateSwapchainKHR(logical_device, &swapchain_info, nullptr, &swapchain);

		mSurface = surface;
		mSwapchain = swapchain;

		// スワップチェインからバックバッファのイメージ取得
		uint32_t image_count = 0;
		result = vkGetSwapchainImagesKHR(logical_device, mSwapchain, &image_count, nullptr);
		std::vector<VkImage> images(image_count);
		result = vkGetSwapchainImagesKHR(logical_device, mSwapchain, &image_count, images.data());
		if (result != VK_SUCCESS)
		{

		}
		// イメージビューの作成
		VkImageViewCreateInfo image_view_info = {};
		image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		image_view_info.format = VK_FORMAT_B8G8R8A8_UNORM;
		image_view_info.components =
		{
			VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A
		};
		image_view_info.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		for (uint32_t i = 0; i < image_count; i++)
		{
			image_view_info.image = images[i];
			VkImageView view = VK_NULL_HANDLE;
			auto result = vkCreateImageView(logical_device, &image_view_info, nullptr, &view);
			if (result != VK_SUCCESS)
			{

			}

			VkSemaphoreCreateInfo semaphore_info = {};
			semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			VkSemaphore semaphore = VK_NULL_HANDLE;
			vkCreateSemaphore(logical_device, &semaphore_info, nullptr, &semaphore);

			VulkanBackBuffer back_buffer = {};
			back_buffer.Image = images[i];
			back_buffer.ImageView = view;
			back_buffer.SignalSemaphore = semaphore;
			mBackBuffers.push_back(back_buffer);
		}
		mMaxFrameCount = image_count;

		SetWidth(swapchain_info.imageExtent.width);
		SetHeight(swapchain_info.imageExtent.height);

	}

#ifdef _WIN64
	void VulkanSwapchain::CreateSurface(HWND hWnd)
	{
		auto instance = GetVkInstance();

		// サーフェイスの作成
		VkWin32SurfaceCreateInfoKHR surfaceInfo{};
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.hinstance = GetModuleHandle(nullptr);
		surfaceInfo.hwnd = hWnd;

		VkSurfaceKHR surface = VK_NULL_HANDLE;
		auto result = vkCreateWin32SurfaceKHR(instance, &surfaceInfo, nullptr, &surface);
		if (result != VK_SUCCESS)
		{
			// device_info extensionsに　VK_KHR_swapchainを追加したか？
		}

		mSurface = surface;

	}
#endif // _WIN64


}
#endif