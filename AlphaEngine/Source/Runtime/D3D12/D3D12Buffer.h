#pragma once
#include "D3D12Device.h"
#include "RHI/RHIBuffer.h"
#if RHI_D3D12

namespace Alpha
{
	class D3D12Buffer : public RHIBuffer, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12Resource1> m_resource{ nullptr };


	public:
		D3D12Buffer(D3D12Device* device);
		~D3D12Buffer();

		bool Initialize(const RHIBufferDesc& desc) final;

		void Write(void* transfer_data) final;

		uint8_t* Map(uint64_t read_begin, uint64_t read_end) final;

		void Unmap(uint64_t written_begin, uint64_t written_end) final;

		void Upload(void* map_ptr, void* data, uint64_t size, uint64_t offset) final;
		
		uint64_t GetGPUAddress() const final;

		void* GetNative() const final { return m_resource.Get(); }
	};

}

#endif