#include "D3D12CommandQueue.h"
#include "D3D12CommandBuffer.h"
#include "RHI/Definitions.h"
#include "D3D12Timeline.h"

#if RHI_D3D12

namespace
{
	static D3D12_COMMAND_LIST_TYPE GetD3D12CommandType(Alpha::ERHICommandType type)
	{
		switch (type)
		{
		case Alpha::ERHICommandType::Direct:	return D3D12_COMMAND_LIST_TYPE_DIRECT;
		case Alpha::ERHICommandType::Bundle:	return D3D12_COMMAND_LIST_TYPE_BUNDLE;
		case Alpha::ERHICommandType::Compute:	return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		case Alpha::ERHICommandType::Copy:		return D3D12_COMMAND_LIST_TYPE_COPY;
		default:
			break;
		}
		return D3D12_COMMAND_LIST_TYPE_DIRECT;
	}

}

namespace Alpha
{
	D3D12CommandQueue::D3D12CommandQueue(D3D12Device* device)
		: RHICommandQueue()
		, D3D12DeviceChild(device)
	{
	}
	
	D3D12CommandQueue::~D3D12CommandQueue()
	{
	}

	bool D3D12CommandQueue::Initialize(const RHICommandQueueDesc& desc)
	{
		HRESULT hr = S_OK;
		D3D12_COMMAND_QUEUE_DESC queue_desc = {};
		queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		queue_desc.Type = GetD3D12CommandType(desc.type);

		auto device = GetD3D12Device();
		//queue_desc.NodeMask = 0;
		queue_desc.NodeMask = device->GetNodeCount();

		hr = device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_queue));

		if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"コマンドキューの作成に失敗しました"))
		{
			return false;
		}
		return true;
	}

	void D3D12CommandQueue::ExecuteCommandBuffer(RHICommandBuffer* command_buffer_)
	{
		auto command_buffer = static_cast<D3D12CommandBuffer*>(command_buffer_);

		
		auto native_list = command_buffer->GetNativeHandle();
		ID3D12CommandList* lists[] = { native_list };

		m_queue->ExecuteCommandLists(1, lists);
		
		//ComPtr<ID3D12DeviceRemovedExtendedData> pDred;
		//if (SUCCEEDED(GetD3D12Device()->QueryInterface(IID_PPV_ARGS(&pDred)))) {
		//	D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT dredAutoBreadcrumbsOutput;
		//	if (SUCCEEDED(pDred->GetAutoBreadcrumbsOutput(&dredAutoBreadcrumbsOutput))) {
		//	}
		//}
	}

	void D3D12CommandQueue::ExecuteCommandBuffers(RHICommandBuffer** command_buffers_, UInt32 command_buffer_count_)
	{
	}

	void D3D12CommandQueue::Signal(RHITimeline* timeline_, UINT64 value)
	{
		auto timeline = static_cast<D3D12Timeline*>(timeline_);
		m_queue->Signal(timeline->GetNativeHandle(), value);
	}
	
	void D3D12CommandQueue::Wait(RHITimeline* timeline_, UINT64 value)
	{
		auto timeline = static_cast<D3D12Timeline*>(timeline_);
		m_queue->Wait(timeline->GetNativeHandle(), value);
	}

	void D3D12CommandQueue::SetName(StringView name)
	{
		m_queue->SetName(name.c_str());
	}

}

#endif
