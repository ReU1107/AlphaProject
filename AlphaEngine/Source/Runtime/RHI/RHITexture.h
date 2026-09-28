#pragma once
#include "Core/Types.h"
#include "RHIResource.h"
#include "Declarations.h"

namespace Alpha
{
	class RHITexture : public RHIResource
	{
	protected:
		uint64_t m_total_upload_size{ 0 };
		UInt32 m_width{ 0 };
		UInt32 m_height{ 0 };
		ERHIResourceFormat m_format{ ERHIResourceFormat::Unknown };
	public:
		virtual ~RHITexture() {}

		virtual bool Initialize(const RHITextureDesc& desc) = 0;

		virtual UInt32 GetSubresourceCount() = 0;

		virtual void Upload(void* map_ptr, const void* data) = 0;

		uint64_t GetUploadSize() const { return m_total_upload_size; }

		// 幅取得
		UInt32 GetWidth() const { return m_width; }
		// 高さ取得
		UInt32 GetHeight() const { return m_height; }
		// フォーマット取得
		ERHIResourceFormat GetFormat() const { return m_format; }

	};
}