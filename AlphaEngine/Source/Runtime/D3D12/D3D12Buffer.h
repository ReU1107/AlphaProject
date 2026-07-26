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

		uint8_t* Map(UInt64 read_begin, UInt64 read_end) final;

		void Unmap(UInt64 written_begin, UInt64 written_end) final;
		
		UInt64 GetGPUAddress() const final;

		void* GetNative() const final { return m_resource.Get(); }

		void SetName(StringView name) final;

	};

}

#endif