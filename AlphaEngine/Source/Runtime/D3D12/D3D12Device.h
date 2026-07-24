#pragma once

#if RHI_D3D12

#include <d3d12.h>
#include <dxgi1_6.h>
#include "Core/Common.h"
#include "RHI/Declarations.h"


namespace Alpha
{
	class D3D12Device
	{
	public:
		D3D12Device();
		~D3D12Device();

		bool Initialize();

		ID3D12Device15* GetDevice() const { return m_device.Get(); }
		IDXGIAdapter4* GetAdapter() const { return m_adapter.Get(); }
		IDXGIFactory7* GetFactory() const { return m_factory.Get(); }

		void RequestTopLevelASSize(const RHITopLevelASDesc& desc, ASData* data);

		void RequestBottomLevelASSize(const RHIBottomLevelASDesc& desc, ASData* data);


	private:

		ComPtr<ID3D12Device15> m_device{ nullptr };

		ComPtr<IDXGIAdapter4> m_adapter{ nullptr };
		ComPtr<IDXGIFactory7> m_factory{ nullptr };

	};

	class D3D12DeviceChild
	{
	public:
		D3D12DeviceChild(D3D12Device* device)
			: m_device(device)
		{

		}
		virtual ~D3D12DeviceChild()
		{
			m_device = nullptr;
		}

	protected:
		D3D12Device* GetDeviceObject() const { return m_device; }
		ID3D12Device15* GetD3D12Device() const { return m_device->GetDevice(); }
		IDXGIAdapter4* GetDXGIAdapter() const { return m_device->GetAdapter(); }
		IDXGIFactory7* GetFactory() const { return m_device->GetFactory(); }


	private:
		D3D12Device* m_device{ nullptr };
	};

}

#endif