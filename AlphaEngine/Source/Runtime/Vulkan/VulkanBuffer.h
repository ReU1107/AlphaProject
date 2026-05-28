#pragma once
#include "VulkanDevice.h"
#include "RHI/RHIBuffer.h"
#if RHI_VK

namespace Alpha
{
	class VulkanBuffer : public RHIBuffer, public VulkanDeviceChild
	{
	private:
		VkDeviceMemory	m_memory{ VK_NULL_HANDLE };
		VkBuffer		m_buffer{ VK_NULL_HANDLE };

		uint32_t		m_memory_size{ 0 };
	public:
		VulkanBuffer(VulkanDevice* device);
		virtual ~VulkanBuffer();

		VkBuffer GetNativeHandle() const { return m_buffer; }

		virtual bool Initialize(const RHIBufferDesc& desc) override final;

		virtual void TransferData(void* transfer_data) override final;
	};
}
#endif