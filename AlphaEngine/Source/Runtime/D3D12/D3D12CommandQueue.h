#pragma once
#include "D3D12Device.h"
#include "RHI/RHICommandQueue.h"

#if RHI_D3D12

namespace Alpha
{
	class D3D12CommandQueue : public RHICommandQueue, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12CommandQueue> m_queue{ nullptr };

	public:
		D3D12CommandQueue(D3D12Device* device);
		~D3D12CommandQueue();

		bool Initialize(const RHICommandQueueDesc& desc) final;

		void ExecuteCommandBuffer(RHICommandBuffer* command_buffer_) final;

		void ExecuteCommandBuffers(std::span<RHICommandBuffer*> command_buffers) final;

		void Signal(RHITimeline* timeline_, UINT64 value) final;

		void Wait(RHITimeline* timeline_, UINT64 value) final;

		void SetName(StringView name) final;

		UInt64 GetTimestampFrequency() final;

		ID3D12CommandQueue* GetNativeHandle() const { return m_queue.Get(); }

	};

}

#endif