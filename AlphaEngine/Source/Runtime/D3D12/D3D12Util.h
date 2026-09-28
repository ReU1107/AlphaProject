#pragma once

#if RHI_D3D12

#include <d3d12.h>
#include <dxgi1_6.h> 
#include <vector>
namespace Alpha
{
	template<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type, typename T>
	struct PipelineStateSubobject
	{
		D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type = Type;
		T data{};
	};

	//https://microsoft.github.io/DirectX-Specs/d3d/DepthBoundsTest.html

	template<typename T>
	void AddSubobject(std::vector<uint8_t>& container, const T& data)
	{
		size_t current = container.size();
		size_t padding = (8 - (current % 8)) % 8;
		container.insert(container.end(), padding, 0);

		const uint8_t* p = reinterpret_cast<const uint8_t*>(&data);
		container.insert(container.end(), p, p + sizeof(T));
	}

    inline DXGI_FORMAT ConvertDXGIFormat(Alpha::ERHIResourceFormat format)
    {
        switch (format)
        {
            // RGBA 32-bit (8-bit per channel)
        case Alpha::ERHIResourceFormat::RGBAUnorm:        return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Alpha::ERHIResourceFormat::RGBAUnormSRGB:    return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case Alpha::ERHIResourceFormat::RGBASnorm:        return DXGI_FORMAT_R8G8B8A8_SNORM;
        case Alpha::ERHIResourceFormat::RGBA32UInt:       return DXGI_FORMAT_R8G8B8A8_UINT;
        case Alpha::ERHIResourceFormat::RGBA32SInt:       return DXGI_FORMAT_R8G8B8A8_SINT;

            // RGBA 64-bit (16-bit per channel)
        case Alpha::ERHIResourceFormat::RGBAHalf:         return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case Alpha::ERHIResourceFormat::RGBA16Float:      return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case Alpha::ERHIResourceFormat::RGBA16Unorm:      return DXGI_FORMAT_R16G16B16A16_UNORM;
        case Alpha::ERHIResourceFormat::RGBA16UInt:       return DXGI_FORMAT_R16G16B16A16_UINT;
        case Alpha::ERHIResourceFormat::RGBA16SInt:       return DXGI_FORMAT_R16G16B16A16_SINT;

            // RGBA 128-bit (32-bit per channel)
        case Alpha::ERHIResourceFormat::RGBAFloat:        return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Alpha::ERHIResourceFormat::RGBA32Float:      return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Alpha::ERHIResourceFormat::RGBA32UInt128:    return DXGI_FORMAT_R32G32B32A32_UINT;
        case Alpha::ERHIResourceFormat::RGBA32SInt128:    return DXGI_FORMAT_R32G32B32A32_SINT;

            // RGB 系統
        case Alpha::ERHIResourceFormat::RGBFloat:         return DXGI_FORMAT_R32G32B32_FLOAT;
        case Alpha::ERHIResourceFormat::RGB10A2Unorm:     return DXGI_FORMAT_R10G10B10A2_UNORM;
        case Alpha::ERHIResourceFormat::R11G11B10Float:   return DXGI_FORMAT_R11G11B10_FLOAT;

            // RG (Dual Channel)
        case Alpha::ERHIResourceFormat::RG8Unorm:         return DXGI_FORMAT_R8G8_UNORM;
        case Alpha::ERHIResourceFormat::RG8UInt:          return DXGI_FORMAT_R8G8_UINT;
        case Alpha::ERHIResourceFormat::RGFloat:          return DXGI_FORMAT_R32G32_FLOAT;
        case Alpha::ERHIResourceFormat::RGUInt:           return DXGI_FORMAT_R32G32_UINT;
        case Alpha::ERHIResourceFormat::RGHalf:           return DXGI_FORMAT_R16G16_FLOAT;
        case Alpha::ERHIResourceFormat::RG16Unorm:        return DXGI_FORMAT_R16G16_UNORM;
        case Alpha::ERHIResourceFormat::RG16UInt:         return DXGI_FORMAT_R16G16_UINT;

            // R (Single Channel)
        case Alpha::ERHIResourceFormat::R8Unorm:          return DXGI_FORMAT_R8_UNORM;
        case Alpha::ERHIResourceFormat::R8UInt:           return DXGI_FORMAT_R8_UINT;
        case Alpha::ERHIResourceFormat::RHalf:            return DXGI_FORMAT_R16_FLOAT;
        case Alpha::ERHIResourceFormat::R16Unorm:         return DXGI_FORMAT_R16_UNORM;
        case Alpha::ERHIResourceFormat::R16UInt:          return DXGI_FORMAT_R16_UINT;
        case Alpha::ERHIResourceFormat::RFloat:           return DXGI_FORMAT_R32_FLOAT;
        case Alpha::ERHIResourceFormat::RUInt:            return DXGI_FORMAT_R32_UINT;

            // BGRA 系統
        case Alpha::ERHIResourceFormat::BGRA8Unorm:       return DXGI_FORMAT_B8G8R8A8_UNORM;
        case Alpha::ERHIResourceFormat::BGRA8UnormSRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

            // デプス / ステンシルフォーマット
        case Alpha::ERHIResourceFormat::Depth32:          return DXGI_FORMAT_D32_FLOAT;
        case Alpha::ERHIResourceFormat::Depth16:          return DXGI_FORMAT_D16_UNORM;
        case Alpha::ERHIResourceFormat::Depth24Stencil8:  return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case Alpha::ERHIResourceFormat::Depth32Stencil8:  return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

            // ブロック圧縮テクスチャ (BC)
        case Alpha::ERHIResourceFormat::BC1Unorm:         return DXGI_FORMAT_BC1_UNORM;
        case Alpha::ERHIResourceFormat::BC1UnormSRGB:     return DXGI_FORMAT_BC1_UNORM_SRGB;
        case Alpha::ERHIResourceFormat::BC2Unorm:         return DXGI_FORMAT_BC2_UNORM;
        case Alpha::ERHIResourceFormat::BC2UnormSRGB:     return DXGI_FORMAT_BC2_UNORM_SRGB;
        case Alpha::ERHIResourceFormat::BC3Unorm:         return DXGI_FORMAT_BC3_UNORM;
        case Alpha::ERHIResourceFormat::BC3UnormSRGB:     return DXGI_FORMAT_BC3_UNORM_SRGB;
        case Alpha::ERHIResourceFormat::BC4Unorm:         return DXGI_FORMAT_BC4_UNORM;
        case Alpha::ERHIResourceFormat::BC4Snorm:         return DXGI_FORMAT_BC4_SNORM;
        case Alpha::ERHIResourceFormat::BC5Unorm:         return DXGI_FORMAT_BC5_UNORM;
        case Alpha::ERHIResourceFormat::BC5Snorm:         return DXGI_FORMAT_BC5_SNORM;
        case Alpha::ERHIResourceFormat::BC6HUFloat:       return DXGI_FORMAT_BC6H_UF16;
        case Alpha::ERHIResourceFormat::BC6HSFloat:       return DXGI_FORMAT_BC6H_SF16;
        case Alpha::ERHIResourceFormat::BC7Unorm:         return DXGI_FORMAT_BC7_UNORM;
        case Alpha::ERHIResourceFormat::BC7UnormSRGB:     return DXGI_FORMAT_BC7_UNORM_SRGB;

        case Alpha::ERHIResourceFormat::Unknown:
        default:
            break;
        }
        return DXGI_FORMAT_UNKNOWN;
    }

}

#endif