#pragma once
#include "VulkanDevice.h"
#include "RHI/RHICommandBuffer.h"
#include <vector>
#if RHI_VK

namespace Alpha
{
	struct VulkanCommand
	{
		VkCommandBuffer Buffer{ VK_NULL_HANDLE };		// コマンドを積むバッファ
		VkSemaphore SignalSemaphore{ VK_NULL_HANDLE };	// 積んだコマンドを処理して待機させる
		//std::vector<VkSemaphore> WaitSemaphores{};			// このコマンドが処理する前に待たないといけないセマフォ群
	};

	class VulkanCommandBuffer : public RHICommandBuffer, public VulkanDeviceChild
	{
	private:
		std::vector<VulkanCommand> mCommands{};

		VkCommandPool mCommandPool{ VK_NULL_HANDLE };
		VkCommandBuffer mCurrentBuffer{ VK_NULL_HANDLE };
		VkSemaphore mCurrentSemaphore{ VK_NULL_HANDLE };
		

	public:
		VulkanCommandBuffer(VulkanDevice* device);
		~VulkanCommandBuffer();

		static void InitializeExtension(VulkanDevice* device);

		VkCommandBuffer GetCommandBuffer(const uint32_t index) const;

		VkCommandBuffer GetCurrentCommandBuffer() const;

		VkSemaphore GetCurrentSemaphore() const;

		virtual void Initialize(const RHICommandBufferDesc& desc) override final;

		virtual void CopyBuffer(RHIBuffer* src, RHIBuffer* dst) override final;

		virtual void Clear(RHISwapchain* swapchain_) override final;

		virtual void Transition(RHISwapchain* swapchain_) override final;

		// 仮想関数
		virtual void Begin(uint32_t) override final;

		virtual void End() override final;

		virtual void BeginRenderTarget(RHISwapchain* swapchain_) override final;
		virtual void BeginRenderTarget(RHITexture* texture_) override final;


		virtual void EndRenderTarget(RHISwapchain* swapchain_) override final;
		virtual void EndRenderTarget(RHITexture* texture_) override final;

		virtual void BeginDynamicRendering(RHISwapchain* swapchain_) override final;

		virtual void EndDynamicRendering() override final;

	};

}
#endif