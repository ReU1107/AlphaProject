#include "DescriptorTable.h"
#include "RenderHardwareInterface.h"
#include "DescriptorHeap.h"
#include "GPUResource.h"
#include "Buffer/Buffer.h"
#include "Texture/Texture.h"

#include "RHI/RHIDescriptorHeap.h"

namespace Alpha
{
	DescriptorTable::DescriptorTable()
	{
	}

	DescriptorTable::~DescriptorTable()
	{
	}

	void DescriptorTable::AddSRV(Buffer* resource)
	{
		Request request = {};
		request.type = ERHIResourceViewType::ShaderResourceView;
		request.resource = resource;
		request.stride = resource->GetStride();

		if (EnumHasFlag(resource->GetFlags(), ERHIBufferBindFlags::Raw))
			request.flags = ERHIResourceViewFlags::ByteAddressBuffer;

		m_requests.push_back(request);
	}

	void DescriptorTable::AddSRV(Texture* resource)
	{
		Request request = {};
		request.type = ERHIResourceViewType::ShaderResourceView;
		request.resource = resource;

		m_requests.push_back(request);
	}

	void DescriptorTable::AddCBV(Buffer* resource)
	{
		Request request = {};
		request.type = ERHIResourceViewType::ConstantBufferView;
		request.resource = resource;
		request.stride = resource->GetStride();

		m_requests.push_back(request);
	}

	void DescriptorTable::AddUAV(Buffer* resource)
	{
		Request request = {};
		request.type = ERHIResourceViewType::UnorderedAccessView;
		request.resource = resource;
		request.stride = resource->GetStride();
		if (EnumHasFlag(resource->GetFlags(), ERHIBufferBindFlags::UnorderedAccess))
		{
			// error
		}
		if (EnumHasFlag(resource->GetFlags(), ERHIBufferBindFlags::Raw))
			request.flags = ERHIResourceViewFlags::ByteAddressBuffer;

		m_requests.push_back(request);
	}

	void DescriptorTable::AddUAV(Texture* resource)
	{
		Request request = {};
		request.type = ERHIResourceViewType::UnorderedAccessView;
		request.resource = resource;

		m_requests.push_back(request);
	}

	void DescriptorTable::Build(RenderHardwareInterface* rhi, DescriptorHeap* descriptor_heap)
	{
		// ここでrequestの数分の空き領域をもらいたい
		m_handle = descriptor_heap->Allocate((UInt32)m_requests.size());

		UInt32 count = 0;
		for (auto& request : m_requests)
		{
			RHIResourceViewDesc view_desc = {};
			view_desc.heap_index = m_handle.index + count++;
			view_desc.resource = request.resource->GetRHI();
			view_desc.type = request.type;
			view_desc.flags = request.flags;
			view_desc.buffer_stride = request.stride;

			m_handle.heap->GetRHI()->CreateView(view_desc);
		}

	}

}