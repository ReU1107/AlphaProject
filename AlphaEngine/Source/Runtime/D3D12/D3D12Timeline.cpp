#include "D3D12Timeline.h"
#include "D3D12CommandQueue.h"


#if RHI_D3D12

namespace Alpha
{
	D3D12Timeline::D3D12Timeline(D3D12Device* device)
		: RHITimeline()
		, D3D12DeviceChild(device)
	{
	}
	
	D3D12Timeline::~D3D12Timeline()
	{
		CloseHandle(m_event);
	}

	bool D3D12Timeline::Initialize(const RHITimelineDesc& desc)
	{
		auto device = GetD3D12Device();

		auto hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
		if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"コマンドフェンスの作成に失敗しました"))
		{
			return false;
		}

		m_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

		return true;
	}

	void D3D12Timeline::WaitCPU(uint64_t wait_value)
	{
		if (m_fence->GetCompletedValue() < wait_value)
		{
			m_fence->SetEventOnCompletion(wait_value, m_event);
			WaitForSingleObject(m_event, INFINITE);
		}
	}
}

#endif