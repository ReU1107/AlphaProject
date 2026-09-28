#include "TextureHandle.h"
#include "Core/Math/MathUtil.h"

namespace Alpha
{
	TextureHandle::~TextureHandle() = default;

	void TextureHandle::SetImage2D(UInt32 width, UInt32 height, ERHIResourceFormat format, TArray<UInt8> pixels, TArray<TextureMipData> mips, ETextureAlphaMode alpha)
	{
		m_width = width;
		m_height = height;
		m_format = format;
		m_alpha_mode = alpha;

		m_pixels = std::move(pixels);
		m_mips = std::move(mips);

		m_dimension = ERHIResourceDimension::Tex2D;
		m_bind_flags = ERHIResourceBindFlags::ShaderResource;
	}

	void TextureHandle::SetRGBA8(UInt32 width, UInt32 height, std::span<const UInt8> pixels, bool srgb)
	{
		const UInt64 row = static_cast<UInt64>(width) * 4;
		const UInt64 size = row * height;

		SetImage2D(width,height,
			srgb ? ERHIResourceFormat::RGBAUnormSRGB : ERHIResourceFormat::RGBAUnorm,
			TArray<UInt8>(pixels.begin(), pixels.end()),
			{
				{
					width,
					height,
					height,
					0,
					row,
					size
				}
			});
	}

	void TextureHandle::SetWidth(UInt32 value)
	{
		m_width = value;
	}

	void TextureHandle::SetHeight(UInt32 value)
	{
		m_height = value;
	}

	void TextureHandle::SetFormat(ERHIResourceFormat value)
	{
		m_format = value;
	}

	void TextureHandle::SetPixels(std::span<const UInt8> pixels)
	{
		const UInt64 row = UInt64(m_width) * 4;
		const UInt64 size = row * m_height;


		SetImage2D(m_width,m_height,m_format,
			TArray<UInt8>(pixels.begin(), pixels.end()),
			{
				{
					m_width,
					m_height,
					m_height,
					0,
					row,
					size
				}
			});
	}

	void TextureHandle::SetPixels(const UInt8* pixels)
	{
		SetPixels(
			std::span<const UInt8>(
			pixels,
			static_cast<std::size_t>(UInt64(m_width) * m_height * 4)));
	}
}