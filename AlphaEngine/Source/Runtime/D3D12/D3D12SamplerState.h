#pragma once
#include "D3D12Device.h"
#include "RHI/RHISamplerState.h"
#if RHI_D3D12

namespace Alpha
{
	class D3D12SamplerState : public RHISamplerState, public D3D12DeviceChild
	{
	private:
		D3D12_CPU_DESCRIPTOR_HANDLE m_cpu_handle{};
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpu_handle{};

	public:
		D3D12SamplerState(D3D12Device* device);
		~D3D12SamplerState();

		bool Initialize(const RHISamplerStateDesc& desc) final;

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const { return m_cpu_handle; }

		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const { return m_gpu_handle; }
	};

}

#endif
