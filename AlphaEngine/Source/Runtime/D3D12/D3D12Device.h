#pragma once

#if RHI_D3D12

#include <d3d12.h>
#include <dxgi1_6.h>
#include "Core/Common.h"


namespace Alpha
{
	class D3D12Device
	{
	public:
		D3D12Device();
		~D3D12Device();

		bool Initialize();

		ID3D12Device15* GetDevice() const { return mDevice.Get(); }
		IDXGIAdapter4* GetAdapter() const { return mAdapter.Get(); }
		IDXGIFactory7* GetFactory() const { return mFactory.Get(); }

	private:

		ComPtr<ID3D12Device15> mDevice{ nullptr };

		ComPtr<IDXGIAdapter4> mAdapter{ nullptr };
		ComPtr<IDXGIFactory7> mFactory{ nullptr };

	};

	class D3D12DeviceChild
	{
	public:
		D3D12DeviceChild(D3D12Device* device)
			: mDevice(device)
		{

		}
		virtual ~D3D12DeviceChild()
		{
			mDevice = nullptr;
		}

	protected:
		D3D12Device* GetDeviceObject() const { return mDevice; }
		ID3D12Device15* GetD3D12Device() const { return mDevice->GetDevice(); }
		IDXGIAdapter4* GetDXGIAdapter() const { return mDevice->GetAdapter(); }
		IDXGIFactory7* GetFactory() const { return mDevice->GetFactory(); }


	private:
		D3D12Device* mDevice{ nullptr };
	};

}

#endif