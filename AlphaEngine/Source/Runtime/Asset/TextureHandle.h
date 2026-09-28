#pragma once
#include "AssetHandle.h"
#include "RHI/Types.h"
#include "Renderer/Types.h"

#include <span>

namespace Alpha
{

    struct TextureMipData 
    {
        UInt32 width{}, height{}, row_count{};
        UInt64 offset{}, row_pitch{}, slice_pitch{};
    };

    class TextureHandle : public AssetHandle 
    {
        UInt32 m_width{ 0 }, m_height{ 0 };
        ERHIResourceDimension m_dimension{ ERHIResourceDimension::Tex2D };
        ERHIResourceFormat m_format{ ERHIResourceFormat::RGBAUnorm };
        ERHIResourceBindFlags m_bind_flags{ ERHIResourceBindFlags::ShaderResource };
        ETextureAlphaMode m_alpha_mode{ ETextureAlphaMode::Unknown };
        TArray<UInt8> m_pixels;
        TArray<TextureMipData> m_mips;

    public:
        ~TextureHandle();

        static AssetType Type() { return AssetType::Texture; }

        AssetType GetType() const final { return Type(); }

        void SetImage2D(UInt32 width, UInt32 height, ERHIResourceFormat format,
            TArray<UInt8> pixels, TArray<TextureMipData> mips,
            ETextureAlphaMode alpha_mode = ETextureAlphaMode::Unknown);

        void SetRGBA8(UInt32 width, UInt32 height, std::span<const UInt8> pixels, bool srgb = false);

        void SetWidth(UInt32 value);
        void SetHeight(UInt32 value);
        void SetFormat(ERHIResourceFormat value);
        void SetPixels(std::span<const UInt8> pixels);
        void SetPixels(const UInt8* pixels);

        UInt32 GetWidth() const { return m_width; }
        UInt32 GetHeight() const { return m_height; }
        UInt32 GetMipCount() const { return static_cast<UInt32>(m_mips.size()); }
        UInt32 GetDepth() const { return 1; }
       
        ERHIResourceDimension GetDimension() const { return m_dimension; }
        ERHIResourceFormat GetFormat() const { return m_format; }
        ERHIResourceBindFlags GetBindFlags() const { return m_bind_flags; }
        ETextureAlphaMode GetAlphaMode() const { return m_alpha_mode; }
        const TArray<UInt8>& GetPixels() const { return m_pixels; }
        UInt64 GetDataSize() const { return m_pixels.size(); }
        const TArray<TextureMipData>& GetMipLayouts() const { return m_mips; }
    };

}