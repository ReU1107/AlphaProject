#pragma once
#include "RenderCore/GPUResource.h"
#include "RHI/Definitions.h"

namespace Alpha
{
	class RHIBuffer;
	class RenderHardwareInterface;
	class DescriptorHeap;

	class Buffer : public GPUResource
	{
	private:
		DescriptorHandle m_srv_handle{};
		DescriptorHandle m_uav_handle{};

	public:
		Buffer();
		~Buffer();

		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			DescriptorHeap* resource_heap{ nullptr };
			UInt64 stride{ 0 };
			UInt64 size{ 0 };
			ERHIResourceAccessType access_type{ ERHIResourceAccessType::Static };
			ERHIResourceBindFlags bind_flags{ ERHIResourceBindFlags::None };
		};

		void Initialize(const Args& args);

		void Destroy(UInt32 current_frame_index) final;

		RHIBuffer* GetRHI() const;

		uint64_t GetStride() const;
		uint64_t GetSize() const;

		UInt8* GetMappedPtr() const;

		DescriptorHandle GetSRVHandle() const { return m_srv_handle; }
		DescriptorHandle GetUAVHandle() const { return m_uav_handle; }
	};

}