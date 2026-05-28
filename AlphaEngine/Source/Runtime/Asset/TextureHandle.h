#pragma once
#include "AssetHandle.h"
#include "RHI/Types.h"

namespace Alpha
{
	class TextureHandle : public AssetHandle
	{
	private:
		uint32_t m_width{ 0 };
		uint32_t m_height{ 0 };

		ERHIResourceFormat m_format{ ERHIResourceFormat::RGBAUnorm };

		TArray<uint8_t> m_pixels{};

	public:
		~TextureHandle();

		static AssetType Type() { return AssetType::Texture; }

		AssetType GetType() const final { return AssetType::Texture; }

		void SetWidth(uint32_t width) { m_width = width; }
		void SetHeight(uint32_t height) { m_height = height; }

		void SetFormat(ERHIResourceFormat format) { m_format = format; }

		void SetPixels(const uint8_t* pixels)
		{
			if (!pixels)
				return;

			size_t size = static_cast<size_t>(m_width) * m_height * 4;

			m_pixels.assign(pixels, pixels + size);
		}

		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }

		ERHIResourceFormat GetFormat() const { return m_format; }


		const uint8_t* GetPixels() const { return m_pixels.data(); }
		uint8_t* GetPixels() { return m_pixels.data(); }

		void Accept(IVisitor* visitor) override {}

	};

}