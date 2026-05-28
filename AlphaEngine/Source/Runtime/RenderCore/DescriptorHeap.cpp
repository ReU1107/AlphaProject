#include "DescriptorHeap.h"
#include "RHI/RHIDescriptorHeap.h"
#include "RenderHardwareInterface.h"

namespace Alpha
{
	DescriptorHeap::DescriptorHeap() = default;
	DescriptorHeap::~DescriptorHeap() = default;

	bool DescriptorHeap::Initialize(const Args& args)
	{
		RHIDescriptorHeapDesc desc = {};
		desc.max_count = args.max_count;
		desc.type = args.type; 

		m_rhi = args.rhi->CreateDescriptorHeap(desc);

		if (!m_rhi)
			return false;

		m_max_count = args.max_count;

		return true;
	}

	RHIDescriptorHeap* DescriptorHeap::GetRHI()
	{
		return m_rhi.get();
	}

	void DescriptorHeap::Finalize()
	{
	}

	DescriptorHandle DescriptorHeap::Allocate()
	{
		UInt32 index = 0;
		// 空きがあればそこを返す
		if (!m_free_indices.empty())
		{
			index = m_free_indices.back();
			m_free_indices.pop_back();
		}
		else 
		{
			ALPHA_CHECK_ERROR((m_allocated_count >= m_max_count), L"デスクリプタヒープの最大数を超えています。");
			index = m_allocated_count++;
		}

		DescriptorHandle handle;
		handle.heap = this;
		handle.index = index;
		handle.num = 1;
		return handle;
	}

	DescriptorHandle DescriptorHeap::Allocate(UInt32 num)
	{
		UInt32 index = 0;

		// 数指定は末尾から限定
		
		ALPHA_CHECK_ERROR((m_allocated_count + num >= m_max_count), L"デスクリプタヒープの最大数を超えています。");

		index = m_allocated_count;
		m_allocated_count += num;

		DescriptorHandle handle;
		handle.heap = this;
		handle.index = index;
		handle.num = num;
		return handle;
	}

	void DescriptorHeap::Free(DescriptorHandle& handle)
	{
		// 後で作成

		handle.heap = nullptr;
		handle.index = 0;
		handle.num = 0;
	}
}