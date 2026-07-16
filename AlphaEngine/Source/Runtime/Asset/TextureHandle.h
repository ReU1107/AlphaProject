#pragma once
#include "AssetHandle.h"
#include "RHI/Types.h"
#include "Renderer/Types.h"

namespace Alpha
{
	class TextureHandle : public AssetHandle
	{
	private:
		UInt32 m_width{ 0 };
		UInt32 m_height{ 0 };
		ERHIResourceDimension m_dimension{ ERHIResourceDimension::Tex2D };
		ERHIResourceFormat m_format{ ERHIResourceFormat::RGBAUnorm };
		ERHIResourceBindFlags m_bind_flags{ ERHIResourceBindFlags::ShaderResource };

		TArray<UInt8> m_pixels{};

	public:
		~TextureHandle();

		static AssetType Type() { return AssetType::Texture; }

		AssetType GetType() const final { return AssetType::Texture; }

		void SetWidth(UInt32 width) { m_width = width; }
		void SetHeight(UInt32 height) { m_height = height; }

		void SetDimension(ERHIResourceDimension dimension) { m_dimension = dimension; }
		void SetFormat(ERHIResourceFormat format) { m_format = format; }
		void SetBindFlags(ERHIResourceBindFlags flags) { m_bind_flags = flags; }

		void SetPixels(const UInt8* pixels)
		{
			if (!pixels)
				return;

			USize size = static_cast<USize>(m_width) * m_height * 4;

			m_pixels.assign(pixels, pixels + size);
		}


		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }

		ERHIResourceDimension GetDimension() const { return m_dimension; }
		ERHIResourceFormat GetFormat() const { return m_format; }
		ERHIResourceBindFlags GetBindFlags() const { return m_bind_flags; }

		const UInt8* GetPixels() const { return m_pixels.data(); }
		UInt8* GetPixels() { return m_pixels.data(); }

		void Accept(IVisitor* visitor) override {}

	};

}