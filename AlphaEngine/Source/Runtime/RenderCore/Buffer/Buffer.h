#pragma once
#include "RenderCore/GPUResource.h"
#include "RHI/Definitions.h"

namespace Alpha
{
	class RHIBuffer;
	class RenderHardwareInterface;

	class Buffer : public GPUResource
	{
	public:
		Buffer();
		virtual ~Buffer();

		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			uint64_t stride{ 0 };
			uint64_t element_count{ 0 };

			// 追加の初期化パラメーターをここに記述できます
		};

		RHIBuffer* GetRHI() const;

		uint64_t GetStride() const;
		uint64_t GetSize() const;

		ERHIBufferBindFlags GetFlags() const;

		void Upload(void* map_ptr, void* data, uint64_t data_size);
	};

}