#include "SwapChain.h"
#include "RenderHardwareInterface.h"
#include "RHI/RHISwapChain.h"

#include "CommandQueue.h"

#include <Windows.h>

namespace Alpha
{
	SwapChain::SwapChain() = default;
	SwapChain::~SwapChain() = default;

	void SwapChain::Initialize(const Args& args)
	{
		RHISwapChainDesc desc = {};
		desc.window_handle = (HWND)args.window_handle;
		desc.buffer_count = args.buffer_count;
		desc.width = args.width;
		desc.height = args.height;
		desc.queue = args.queue->GetRHI();

		m_rhi = args.rhi->CreateSwapChain(desc);
	}

	RHISwapChain* SwapChain::GetRHI() const
	{
		return m_rhi.get();
	}

	UInt32 SwapChain::GetCurrentBackBufferIndex() const
	{
		return m_rhi->GetCurrentBackBufferIndex();
	}

	void SwapChain::Present()
	{
		m_rhi->Present();
	}
}