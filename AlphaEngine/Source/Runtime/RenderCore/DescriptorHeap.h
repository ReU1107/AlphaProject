#pragma once
#include "Core/Common.h"
#include "RHI/Types.h"
#include "DescriptorHandle.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class RHIDescriptorHeap;

	class DescriptorHeap
	{
	private:
		TUnique<RHIDescriptorHeap> m_rhi{ nullptr };
		TArray<UInt64> m_free_indices{};

		UInt64 m_max_count{ 0 };          // 最大数
		UInt64 m_allocated_count{ 0 };    // 使用済み数

	public:
		DescriptorHeap();
		~DescriptorHeap();
		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			ERHIDescriptorType type{ ERHIDescriptorType::SRV_UAV_CBV };
			UInt64 max_count{ 0 };

			// 追加の初期化パラメーターをここに記述できます
		};
		bool Initialize(const Args& args);
		RHIDescriptorHeap* GetRHI();

		void Finalize();

		DescriptorHandle Allocate();
		DescriptorHandle Allocate(UInt32 num);

		void Free(DescriptorHandle& handle);



	};
}