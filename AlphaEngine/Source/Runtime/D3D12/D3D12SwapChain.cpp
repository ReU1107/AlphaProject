#include "D3D12SwapChain.h"
#include "D3D12CommandQueue.h"
#include "RHI/Definitions.h"

#if RHI_D3D12

#include <dxgi.h>

namespace Alpha
{
	D3D12SwapChain::D3D12SwapChain(D3D12Device* device)
		: RHISwapChain()
		, D3D12DeviceChild(device)
	{
	}

	D3D12SwapChain::~D3D12SwapChain()
	{
	}

	bool D3D12SwapChain::Initialize(const RHISwapChainDesc& desc)
	{
		DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
		swap_chain_desc.BufferCount = desc.buffer_count;
		swap_chain_desc.Width = desc.width;
		swap_chain_desc.Height = desc.height;
		swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swap_chain_desc.SampleDesc.Count = 1;

		ComPtr<IDXGISwapChain1> swap_chain = nullptr;

		auto command_queue = static_cast<D3D12CommandQueue*>(desc.queue)->GetNativeHandle();

		auto factory = GetFactory();

		auto hr = GetFactory()->CreateSwapChainForHwnd(
			command_queue,
			desc.window_handle,
			&swap_chain_desc,
			nullptr,
			nullptr,
			&swap_chain
		);

		if (ALPHA_CHECK_ERROR(FAILED(hr), L"スワップチェインの作成に失敗しました"))
		{
			return false;
		}

		hr = swap_chain.As(&m_swap_chain);
		if (ALPHA_CHECK_ERROR(FAILED(hr), L"スワップチェインの作成に失敗しました"))
		{
			return false;
		}

		return true;
	}

	void D3D12SwapChain::Present()
	{
		m_swap_chain->Present(0, 0);
	}

	UInt32 D3D12SwapChain::GetCurrentBackBufferIndex() const
	{
		return m_swap_chain->GetCurrentBackBufferIndex();
	}

	ID3D12Resource1* D3D12SwapChain::GetBackBuffer(UInt32 index) const
	{
		ID3D12Resource1* back_buffer = nullptr;
		m_swap_chain->GetBuffer(index, IID_PPV_ARGS(&back_buffer));
		back_buffer->SetName(L"SwapChain BackBuffer");
		return back_buffer;
	}
}

#endif