#include "DDSLoader.h"
#include "Asset/TextureHandle.h"

#define TINYDDSLOADER_IMPLEMENTATION
#include <tinyddsloader/tinyddsloader.h>

namespace Alpha
{
	static ERHIResourceFormat ConvertFormat(tinyddsloader::DDSFile::DXGIFormat format)
	{
		using DXGIFormat = tinyddsloader::DDSFile::DXGIFormat;

		switch (format)
		{
			// ------------------------------------------------------------
			// RGBA 128-bit
			// ------------------------------------------------------------
		case DXGIFormat::R32G32B32A32_Float:
			return ERHIResourceFormat::RGBA32Float;

		case DXGIFormat::R32G32B32A32_UInt:
			return ERHIResourceFormat::RGBA32UInt128;

		case DXGIFormat::R32G32B32A32_SInt:
			return ERHIResourceFormat::RGBA32SInt128;

			// ------------------------------------------------------------
			// RGB 96-bit
			// ------------------------------------------------------------
		case DXGIFormat::R32G32B32_Float:
			return ERHIResourceFormat::RGBFloat;

			// ------------------------------------------------------------
			// RGBA 64-bit
			// ------------------------------------------------------------
		case DXGIFormat::R16G16B16A16_Float:
			return ERHIResourceFormat::RGBA16Float;

		case DXGIFormat::R16G16B16A16_UNorm:
			return ERHIResourceFormat::RGBA16Unorm;

		case DXGIFormat::R16G16B16A16_UInt:
			return ERHIResourceFormat::RGBA16UInt;

		case DXGIFormat::R16G16B16A16_SInt:
			return ERHIResourceFormat::RGBA16SInt;

			// ERHIResourceFormat側にRGBA16Snormがない
		case DXGIFormat::R16G16B16A16_SNorm:
			return ERHIResourceFormat::Unknown;

			// ------------------------------------------------------------
			// RGBA 32-bit
			// ------------------------------------------------------------
		case DXGIFormat::R8G8B8A8_UNorm:
			return ERHIResourceFormat::RGBAUnorm;

		case DXGIFormat::R8G8B8A8_UNorm_SRGB:
			return ERHIResourceFormat::RGBAUnormSRGB;

		case DXGIFormat::R8G8B8A8_UInt:
			return ERHIResourceFormat::RGBA32UInt;

		case DXGIFormat::R8G8B8A8_SNorm:
			return ERHIResourceFormat::RGBASnorm;

		case DXGIFormat::R8G8B8A8_SInt:
			return ERHIResourceFormat::RGBA32SInt;

			// ------------------------------------------------------------
			// RGB packed
			// ------------------------------------------------------------
		case DXGIFormat::R10G10B10A2_UNorm:
			return ERHIResourceFormat::RGB10A2Unorm;

		case DXGIFormat::R11G11B10_Float:
			return ERHIResourceFormat::R11G11B10Float;

			// ------------------------------------------------------------
			// RG 64-bit
			// ------------------------------------------------------------
		case DXGIFormat::R32G32_Float:
			return ERHIResourceFormat::RGFloat;

		case DXGIFormat::R32G32_UInt:
			return ERHIResourceFormat::RGUInt;

			// ------------------------------------------------------------
			// RG 32-bit
			// ------------------------------------------------------------
		case DXGIFormat::R16G16_Float:
			return ERHIResourceFormat::RGHalf;

		case DXGIFormat::R16G16_UNorm:
			return ERHIResourceFormat::RG16Unorm;

		case DXGIFormat::R16G16_UInt:
			return ERHIResourceFormat::RG16UInt;

			// ------------------------------------------------------------
			// RG 16-bit
			// ------------------------------------------------------------
		case DXGIFormat::R8G8_UNorm:
			return ERHIResourceFormat::RG8Unorm;

		case DXGIFormat::R8G8_UInt:
			return ERHIResourceFormat::RG8UInt;

			// ------------------------------------------------------------
			// R 32-bit
			// ------------------------------------------------------------
		case DXGIFormat::R32_Float:
			return ERHIResourceFormat::RFloat;

		case DXGIFormat::R32_UInt:
			return ERHIResourceFormat::RUInt;

			// ------------------------------------------------------------
			// R 16-bit
			// ------------------------------------------------------------
		case DXGIFormat::R16_Float:
			return ERHIResourceFormat::RHalf;

		case DXGIFormat::R16_UNorm:
			return ERHIResourceFormat::R16Unorm;

		case DXGIFormat::R16_UInt:
			return ERHIResourceFormat::R16UInt;

			// ------------------------------------------------------------
			// R 8-bit
			// ------------------------------------------------------------
		case DXGIFormat::R8_UNorm:
			return ERHIResourceFormat::R8Unorm;

		case DXGIFormat::R8_UInt:
			return ERHIResourceFormat::R8UInt;

			// ------------------------------------------------------------
			// BGRA
			// ------------------------------------------------------------
		case DXGIFormat::B8G8R8A8_UNorm:
			return ERHIResourceFormat::BGRA8Unorm;

		case DXGIFormat::B8G8R8A8_UNorm_SRGB:
			return ERHIResourceFormat::BGRA8UnormSRGB;

			// ------------------------------------------------------------
			// Depth / Stencil
			// ------------------------------------------------------------
		case DXGIFormat::D32_Float:
			return ERHIResourceFormat::Depth32;

		case DXGIFormat::D16_UNorm:
			return ERHIResourceFormat::Depth16;

		case DXGIFormat::D24_UNorm_S8_UInt:
			return ERHIResourceFormat::Depth24Stencil8;

		case DXGIFormat::D32_Float_S8X24_UInt:
			return ERHIResourceFormat::Depth32Stencil8;

			// ------------------------------------------------------------
			// BC1
			// ------------------------------------------------------------
		case DXGIFormat::BC1_UNorm:
			return ERHIResourceFormat::BC1Unorm;

		case DXGIFormat::BC1_UNorm_SRGB:
			return ERHIResourceFormat::BC1UnormSRGB;

			// ------------------------------------------------------------
			// BC2
			// ------------------------------------------------------------
		case DXGIFormat::BC2_UNorm:
			return ERHIResourceFormat::BC2Unorm;

		case DXGIFormat::BC2_UNorm_SRGB:
			return ERHIResourceFormat::BC2UnormSRGB;

			// ------------------------------------------------------------
			// BC3
			// ------------------------------------------------------------
		case DXGIFormat::BC3_UNorm:
			return ERHIResourceFormat::BC3Unorm;

		case DXGIFormat::BC3_UNorm_SRGB:
			return ERHIResourceFormat::BC3UnormSRGB;

			// ------------------------------------------------------------
			// BC4
			// ------------------------------------------------------------
		case DXGIFormat::BC4_UNorm:
			return ERHIResourceFormat::BC4Unorm;

		case DXGIFormat::BC4_SNorm:
			return ERHIResourceFormat::BC4Snorm;

			// ------------------------------------------------------------
			// BC5
			// ------------------------------------------------------------
		case DXGIFormat::BC5_UNorm:
			return ERHIResourceFormat::BC5Unorm;

		case DXGIFormat::BC5_SNorm:
			return ERHIResourceFormat::BC5Snorm;

			// ------------------------------------------------------------
			// BC6H
			// ------------------------------------------------------------
		case DXGIFormat::BC6H_UF16:
			return ERHIResourceFormat::BC6HUFloat;

		case DXGIFormat::BC6H_SF16:
			return ERHIResourceFormat::BC6HSFloat;

			// ------------------------------------------------------------
			// BC7
			// ------------------------------------------------------------
		case DXGIFormat::BC7_UNorm:
			return ERHIResourceFormat::BC7Unorm;

		case DXGIFormat::BC7_UNorm_SRGB:
			return ERHIResourceFormat::BC7UnormSRGB;

		default:
			return ERHIResourceFormat::Unknown;
		}
	}

    TShared<AssetHandle> DDSLoader::AsyncLoad(StringView file_path)
    {
        tinyddsloader::DDSFile dds;

        auto result = dds.Load("texture.dds");
        if (result != tinyddsloader::Result::Success)
        {
            // 読み込み失敗
            return nullptr;
        }
        UInt32 width = dds.GetWidth();
        UInt32 height = dds.GetHeight();
        UInt32 depth = dds.GetDepth();
        UInt32 mip_count = dds.GetMipCount();
        UInt32 array_size = dds.GetArraySize();

        auto dimension = dds.GetTextureDimension();


        TArray<UInt8> pixels;
        TArray<TextureMipData> mips;

        UInt64 total_size = 0;

        for (UInt32 mip = 0; mip < mip_count; ++mip)
        {
            const auto* image = dds.GetImageData(mip, 0);

            total_size += image->m_memSlicePitch;
        }

        pixels.resize(total_size);
        mips.reserve(mip_count);

        UInt64 offset = 0;

        for (UInt32 mip = 0; mip < mip_count; ++mip)
        {
            const auto* image = dds.GetImageData(mip, 0);

            memcpy(
                pixels.data() + offset,
                image->m_mem,
                image->m_memSlicePitch);

            TextureMipData mip_data{};

            mip_data.width = image->m_width;
            mip_data.height = image->m_height;
            mip_data.offset = offset;
            mip_data.row_pitch = image->m_memPitch;
            mip_data.slice_pitch = image->m_memSlicePitch;
            mip_data.row_count =
                image->m_memPitch > 0
                ? image->m_memSlicePitch / image->m_memPitch
                : 0;

            mips.push_back(mip_data);

            offset += image->m_memSlicePitch;
        }

        auto texture = std::make_shared<TextureHandle>();

		const ERHIResourceFormat format = ConvertFormat(dds.GetFormat());

		if (format == ERHIResourceFormat::Unknown)
		{
			ALPHA_LOG_WARN(L"DDSLoader: unsupported DXGI format");
			return nullptr;
		}

		texture->SetImage2D(
			width,
			height,
			format,
			std::move(pixels),
			std::move(mips));
    }
}
