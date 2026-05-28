#include "VulkanCommandBuffer.h"

#include "VulkanSwapchain.h"
#include "VulkanBuffer.h"
#include "RHI/RHIDefinitions.h"

#if RHI_VK

namespace Alpha
{
	PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR{ VK_NULL_HANDLE };
	PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR{ VK_NULL_HANDLE };


	VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device)
		: RHICommandBuffer()
		, VulkanDeviceChild(device)
	{

	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		auto device = GetVkLogicalDevice();
		
		std::vector<VkCommandBuffer> command_buffers(mCommands.size());

		for (uint32_t i = 0; i < command_buffers.size(); i++)
		{
			command_buffers[i] = mCommands.at(i).Buffer;
			//vkDestroySemaphore(device, mCommands.at(i).SignalSemaphore, nullptr);
		}

		vkFreeCommandBuffers(device, mCommandPool, command_buffers.size(), command_buffers.data());
		vkDestroyCommandPool(device, mCommandPool, nullptr);
	}

	static bool IsExtensionInitialized = false;

	void VulkanCommandBuffer::InitializeExtension(VulkanDevice* device_)
	{
		// 既に初期化済み
		if (IsExtensionInitialized)
			return;
		auto device = device_->GetVkDevice();
		vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderingKHR"));
		vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(vkGetDeviceProcAddr(device, "vkCmdEndRenderingKHR"));
		IsExtensionInitialized = true;

	}

	VkCommandBuffer VulkanCommandBuffer::GetCommandBuffer(const uint32_t index) const
	{
		return mCommands.at(index).Buffer;
	}

	VkCommandBuffer VulkanCommandBuffer::GetCurrentCommandBuffer() const
	{
		return mCurrentBuffer;
	}

	VkSemaphore VulkanCommandBuffer::GetCurrentSemaphore() const
	{
		return mCurrentSemaphore;
	}

	void VulkanCommandBuffer::Initialize(const RHICommandBufferDesc& desc)
	{
		auto device = GetVkLogicalDevice();

		uint32_t index = GetGraphicsQueueFamilyIndex();
		uint32_t count = desc.Count;

		VkCommandPoolCreateInfo command_pool_info = {};
		command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		command_pool_info.queueFamilyIndex = index;
		VkCommandPool command_pool = VK_NULL_HANDLE;
		auto result = vkCreateCommandPool(device, &command_pool_info, nullptr, &command_pool);
		if (result != VK_SUCCESS)
		{

		}

		VkCommandBufferAllocateInfo command_buffer_info = {};
		command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_info.commandPool = command_pool;
		command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_info.commandBufferCount = count;

		std::vector<VkCommandBuffer> command_buffers(count);
		result = vkAllocateCommandBuffers(device, &command_buffer_info, command_buffers.data());
		if (result != VK_SUCCESS)
		{

		}

		mCommands.resize(count);
		for (uint32_t i = 0; i < count; i++)
		{
			//VkSemaphoreCreateInfo semaphore_info = {};
			//VkSemaphore semaphore = VK_NULL_HANDLE;
			//vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore);

			mCommands[i].Buffer = command_buffers[i];
			//mCommands[i].SignalSemaphore = semaphore;
		}

		mCommandPool = command_pool;
	}

	void VulkanCommandBuffer::CopyBuffer(RHIBuffer* src, RHIBuffer* dst)
	{
		auto dst_buffer = static_cast<VulkanBuffer*>(dst);
		auto src_buffer = static_cast<VulkanBuffer*>(src);

		auto dst_native = dst_buffer->GetNativeHandle();
		auto src_native = src_buffer->GetNativeHandle();

		VkBufferCopy copy_region = {};

		copy_region.size = dst_buffer->GetSize();
		VkCommandBuffer command_buffer = mCurrentBuffer;

		vkCmdCopyBuffer(command_buffer, src_native, dst_native, 1, &copy_region);
	}

	void VulkanCommandBuffer::Clear(RHISwapchain* swapchain_)
	{
		VkCommandBuffer command_buffer = mCurrentBuffer;

		auto swapchain = static_cast<VulkanSwapchain*>(swapchain_);
		auto image = swapchain->GetCurrentImage();
		auto& back_buffers = swapchain->GetBackBuffers();
		auto& back_buffer = back_buffers.at(swapchain->GetCurrentImageIndex());

		VkImageSubresourceRange subresource_range = back_buffer.mSubresourceRange;
		VkClearColorValue clear_color = {};
		clear_color.float32[0] = 1.0f;
		clear_color.float32[1] = 0.0f;
		clear_color.float32[2] = 1.0f;
		clear_color.float32[3] = 1.0f;

		vkCmdClearColorImage(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clear_color, 1, &subresource_range);

	}

	void VulkanCommandBuffer::Transition(RHISwapchain* swapchain_)
	{
		VkCommandBuffer command_buffer = mCurrentBuffer;
		VkPipelineStageFlags src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		VkPipelineStageFlags dst = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		auto swapchain = static_cast<VulkanSwapchain*>(swapchain_);
		auto& back_buffers = swapchain->GetBackBuffers();
		auto& back_buffer = back_buffers.at(swapchain->GetCurrentImageIndex());

		auto old_layout = back_buffer.CurrentLayout;
		auto new_layout = back_buffer.NextLayout;

		auto image = back_buffer.Image;
		VkAccessFlags src_mask = 0;
		VkAccessFlags dst_mask = 0;

		if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED || old_layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
		{
			new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			new_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			dst_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		else if(old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			new_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			src_mask = VK_ACCESS_TRANSFER_READ_BIT;
		}

		VkImageSubresourceRange subresource_range = {};
		subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresource_range.levelCount = 1;
		subresource_range.layerCount = 1;


		VkImageMemoryBarrier image_barrior = {};
		image_barrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		image_barrior.oldLayout = old_layout;
		image_barrior.newLayout = new_layout;
		image_barrior.image = image;
		image_barrior.subresourceRange = subresource_range;
		
		vkCmdPipelineBarrier(command_buffer, src, dst, VkDependencyFlags(), 0, nullptr, 0, nullptr, 1, &image_barrior);

		back_buffer.CurrentLayout = new_layout;
		back_buffer.mSubresourceRange = subresource_range;

	}

	void VulkanCommandBuffer::Begin(uint32_t index)
	{
		VkCommandBuffer command_buffer = GetCommandBuffer(index);
		
		auto result = vkResetCommandBuffer(command_buffer, 0);
		if (result != VK_SUCCESS)
		{

		}

		VkCommandBufferBeginInfo begine_info = {};
		begine_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		result = vkBeginCommandBuffer(command_buffer, &begine_info);

		if (result != VK_SUCCESS)
		{

		}

		mCurrentBuffer = command_buffer;

		//mCurrentSemaphore = mCommands.at(index).SignalSemaphore;
	}

	void VulkanCommandBuffer::End()
	{
		VkCommandBuffer command_buffer = GetCurrentCommandBuffer();

		auto result = vkEndCommandBuffer(command_buffer);
		if (result != VK_SUCCESS)
		{

		}
	}

	void VulkanCommandBuffer::BeginRenderTarget(RHISwapchain* swapchain_)
	{
		VkCommandBuffer command_buffer = GetCurrentCommandBuffer();

		auto swapchain = static_cast<VulkanSwapchain*>(swapchain_);
		auto image = swapchain->GetCurrentImage();
		auto image_view = swapchain->GetCurrentImageView();
		uint32_t width = swapchain->GetWidth();
		uint32_t height = swapchain->GetHeight();

		VkImageSubresourceRange subresource_range = {};
		subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresource_range.levelCount = 1;
		subresource_range.layerCount = 1;

		VkImageMemoryBarrier image_barrior = {};
		image_barrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		image_barrior.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_barrior.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		image_barrior.image = image;
		image_barrior.subresourceRange = subresource_range;

		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkDependencyFlags(), 0, nullptr, 0, nullptr, 1, &image_barrior);

	}

	void VulkanCommandBuffer::BeginRenderTarget(RHITexture* texture_)
	{
	}

	void VulkanCommandBuffer::EndRenderTarget(RHISwapchain* swapchain_)
	{
		VkCommandBuffer command_buffer = GetCurrentCommandBuffer();

		auto swapchain = static_cast<VulkanSwapchain*>(swapchain_);
		auto image = swapchain->GetCurrentImage();
		auto image_view = swapchain->GetCurrentImageView();
		uint32_t width = swapchain->GetWidth();
		uint32_t height = swapchain->GetHeight();

		VkImageSubresourceRange subresource_range = {};
		subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		subresource_range.levelCount = 1;
		subresource_range.layerCount = 1;

		VkImageMemoryBarrier image_barrior = {};
		image_barrior.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		image_barrior.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		image_barrior.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		image_barrior.image = image;
		image_barrior.subresourceRange = subresource_range;

		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VkDependencyFlags(), 0, nullptr, 0, nullptr, 1, &image_barrior);

	}

	void VulkanCommandBuffer::EndRenderTarget(RHITexture* texture_)
	{
	}

	void VulkanCommandBuffer::BeginDynamicRendering(RHISwapchain* swapchain_)
	{
		VkCommandBuffer command_buffer = GetCurrentCommandBuffer();

		auto swapchain = static_cast<VulkanSwapchain*>(swapchain_);
		auto image = swapchain->GetCurrentImage();
		auto image_view = swapchain->GetCurrentImageView();
		uint32_t width = swapchain->GetWidth();
		uint32_t height = swapchain->GetHeight();

		VkRenderingAttachmentInfoKHR color_attachment = {};
		color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		color_attachment.imageView = image_view;
		color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.clearValue.color = { 1.0f,0.0f,0.0f,1.0f };

		//// A single depth stencil attachment info can be used, but they can also be specified separately.
		//// When both are specified separately, the only requirement is that the image view is identical.			
		//VkRenderingAttachmentInfoKHR depthStencilAttachment{};
		//depthStencilAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		//depthStencilAttachment.imageView = depthStencil.view;
		//depthStencilAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		//depthStencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//depthStencilAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		//depthStencilAttachment.clearValue.depthStencil = { 1.0f,  0 };

		VkRenderingInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
		info.renderArea = { 0, 0, width, height };
		info.layerCount = 1;
		info.colorAttachmentCount = 1;
		info.pColorAttachments = &color_attachment;
		//renderingInfo.pDepthAttachment = &depthStencilAttachment;
		//renderingInfo.pStencilAttachment = &depthStencilAttachment;

		vkCmdBeginRenderingKHR(command_buffer, &info);


		VkViewport viewport = {};
		viewport.width = (float)width; 
		viewport.height = (float)height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(command_buffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = width;
		scissor.extent.height = height;
		vkCmdSetScissor(command_buffer, 0, 1, &scissor);

	}

	void VulkanCommandBuffer::EndDynamicRendering()
	{
		VkCommandBuffer command_buffer = GetCurrentCommandBuffer();

		vkCmdEndRenderingKHR(command_buffer);
	}
}
#endif