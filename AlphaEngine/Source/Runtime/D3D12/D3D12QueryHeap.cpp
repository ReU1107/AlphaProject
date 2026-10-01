#include "D3D12QueryHeap.h"
#include "RHI/Definitions.h"


#if RHI_D3D12

namespace
{
	inline D3D12_QUERY_HEAP_TYPE ConvertD3D12QueryHeapType(Alpha::ERHIQueryType rhiType)
	{
		switch (rhiType)
		{
		case Alpha::ERHIQueryType::Timestamp:          return D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
		case Alpha::ERHIQueryType::Occlusion:          return D3D12_QUERY_HEAP_TYPE_OCCLUSION;
		case Alpha::ERHIQueryType::BinaryOcclusion:    return D3D12_QUERY_HEAP_TYPE_OCCLUSION; // Heapは通常Occlusionと共通
		case Alpha::ERHIQueryType::PipelineStatistics: return D3D12_QUERY_HEAP_TYPE_PIPELINE_STATISTICS;
		default: throw std::invalid_argument("Invalid ERHIQueryType");
		}
	}
}

namespace Alpha
{
	D3D12QueryHeap::D3D12QueryHeap(D3D12Device* device)
		: RHIQueryHeap()
		, D3D12DeviceChild(device)
	{
	}

	D3D12QueryHeap::~D3D12QueryHeap()
	{
	}

	bool D3D12QueryHeap::Initialize(const RHIQueryHeapDesc& desc)
	{
		auto device = GetD3D12Device();

		D3D12_QUERY_HEAP_DESC heap_desc = {};
		heap_desc.Count = desc.count;
		heap_desc.Type = ConvertD3D12QueryHeapType(desc.type); // タイムスタンプ用
		heap_desc.NodeMask = 0;
		
		auto hr = device->CreateQueryHeap(&heap_desc, IID_PPV_ARGS(&m_heap));
		
		if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"の作成に失敗しました"))
		{
			return false;
		}

		return true;
	}

}

#endif