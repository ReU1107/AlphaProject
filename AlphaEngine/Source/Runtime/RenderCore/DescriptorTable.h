#pragma once
#include "Core/Common.h"
#include "DescriptorHandle.h"
#include "RHI/Types.h"

namespace Alpha
{
	class RenderHardwareInterface;

	class GPUResource;
	class Buffer;
	class Texture;
	class DescriptorHeap;

	struct Request
	{
		GPUResource* resource{ nullptr };
		UInt64 stride{ 0 };
		ERHIResourceViewType type{ ERHIResourceViewType::Unknown };
		ERHIResourceViewFlags flags{ ERHIResourceViewFlags::None };
	};

	class DescriptorTable
	{
	private:
		
		TArray<Request> m_requests{};
		DescriptorHandle m_handle{}; // SetRootDescriptorTable()に渡す様の先頭アドレスを持ったクラス

	public:
		DescriptorTable();
		~DescriptorTable();

		void AddSRV(Buffer* resource);
		void AddSRV(Texture* resource);

		void AddCBV(Buffer* resource);

		void AddUAV(Buffer* resource);
		void AddUAV(Texture* resource);

		void Build(RenderHardwareInterface* rhi, DescriptorHeap* heap);

		const DescriptorHandle& GetHandle() const { return m_handle; }

	};

}