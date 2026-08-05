#include "D3D12Texture.h"
#include "D3D12SwapChain.h"
#include "D3D12Util.h"
#include "RHI/Definitions.h"

#if RHI_D3D12

namespace
{
	static D3D12_RESOURCE_DIMENSION ConvertD3D12ResourceDimension(Alpha::ERHIResourceDimension dimension)
	{
		switch (dimension)
		{
		case Alpha::ERHIResourceDimension::Unknown		: return D3D12_RESOURCE_DIMENSION_UNKNOWN;
		case Alpha::ERHIResourceDimension::None			: return D3D12_RESOURCE_DIMENSION_UNKNOWN;
		case Alpha::ERHIResourceDimension::Any			: return D3D12_RESOURCE_DIMENSION_UNKNOWN;
		case Alpha::ERHIResourceDimension::Tex1D		: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case Alpha::ERHIResourceDimension::Tex2D		: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case Alpha::ERHIResourceDimension::Tex3D		: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		case Alpha::ERHIResourceDimension::Cube			: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case Alpha::ERHIResourceDimension::Tex2DArray	: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case Alpha::ERHIResourceDimension::CubeArray	: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		default:
			break;
		}
		return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}

	D3D12_RESOURCE_FLAGS ConvertD3D12ResourceFlags(Alpha::ERHIResourceBindFlags flags)
	{
		D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE;

		if (EnumHasFlag(flags, Alpha::ERHIResourceBindFlags::RenderTarget))
		{
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}
		if (EnumHasFlag(flags, Alpha::ERHIResourceBindFlags::DepthStencil))
		{
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			if (!EnumHasFlag(flags, Alpha::ERHIResourceBindFlags::ShaderResource))
			{
				resourceFlags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
			}
		}
		if (EnumHasFlag(flags, Alpha::ERHIResourceBindFlags::UnorderedAccess))
		{
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}


		return resourceFlags;
	}

}

namespace Alpha
{
	D3D12Texture::D3D12Texture(D3D12Device* device)
		: RHITexture()
		, D3D12DeviceChild(device)
	{
	}
	
	D3D12Texture::~D3D12Texture()
	{
	}

	bool D3D12Texture::Initialize(const RHITextureDesc& desc)
	{
		if (desc.swap_chain)
		{
			// SwapChainから作成する場合

			auto d3d12_swap_chain = static_cast<D3D12SwapChain*>(desc.swap_chain);

			m_resource = d3d12_swap_chain->GetBackBuffer(desc.back_buffer_index);
			if (!m_resource)
			{
				return false;
			}
			
			SetState(ERHIResourceState::Present);
			m_format = ERHIResourceFormat::RGBAUnorm;
			m_width = m_resource->GetDesc().Width;
			m_height = m_resource->GetDesc().Height;
		}
		else
		{
			D3D12_HEAP_PROPERTIES prop = {};
			prop.Type = D3D12_HEAP_TYPE_DEFAULT;
			prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			prop.CreationNodeMask = 1;
			prop.VisibleNodeMask = 1;

			D3D12_RESOURCE_DIMENSION dimension = ConvertD3D12ResourceDimension(desc.dimension);

			D3D12_RESOURCE_FLAGS resource_flags = ConvertD3D12ResourceFlags(desc.flags);

			DXGI_FORMAT format = ConvertDXGIFormat(desc.format);

			D3D12_RESOURCE_DESC resource_desc = {};
			resource_desc.Dimension = dimension;
			resource_desc.Alignment = 0;
			resource_desc.Width = desc.width;
			resource_desc.Height = desc.height;
			resource_desc.DepthOrArraySize = desc.depth;
			resource_desc.MipLevels = desc.mip_level;
			resource_desc.Format = format;
			resource_desc.SampleDesc.Count = desc.multi_sample_count;
			resource_desc.SampleDesc.Quality = desc.multi_sample_quality;
			resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			resource_desc.Flags = resource_flags;

			D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS feature = {};
			feature.Format = format;
			feature.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
			feature.NumQualityLevels = desc.multi_sample_quality;
			feature.SampleCount = desc.multi_sample_count;

			auto device = GetD3D12Device();

			auto hr = device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &feature, sizeof(feature));

			D3D12_CLEAR_VALUE clear_value = {};
			clear_value.Format = format;

			if (EnumHasFlag(desc.flags, ERHIResourceBindFlags::DepthStencil))
			{
				clear_value.DepthStencil.Depth = desc.clear_value_depth;
				clear_value.DepthStencil.Stencil = desc.clear_value_stencil;
				hr = device->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_COMMON, &clear_value, IID_PPV_ARGS(&m_resource));
			}
			else if (EnumHasFlag(desc.flags, ERHIResourceBindFlags::RenderTarget))
			{
				clear_value.Color[0] = desc.clear_value_color[0];
				clear_value.Color[1] = desc.clear_value_color[1];
				clear_value.Color[2] = desc.clear_value_color[2];
				clear_value.Color[3] = desc.clear_value_color[3];
				hr = device->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_COMMON, &clear_value, IID_PPV_ARGS(&m_resource));
			}
			else
			{
				hr = device->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&m_resource));
		
				/*device->CreatePlacedResource();
				device->CreateReservedResource();
				がちがちに最適化するならここら辺を使うことになるのかな？
				*/
				// コピー用のデータ用意(RT,DSはいらない)
				UInt32 subresource_num = 0;

				if (resource_desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE3D)
				{
					subresource_num = resource_desc.MipLevels;
				}
				else
				{
					subresource_num = resource_desc.MipLevels * resource_desc.DepthOrArraySize;
				}

				m_layouts.resize(subresource_num);
				std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprints(subresource_num);
				std::vector<UINT> row_counts(subresource_num);
				std::vector<UINT64> row_sizes(subresource_num);

				device->GetCopyableFootprints(&resource_desc, 0, subresource_num, 0, footprints.data(), row_counts.data(), row_sizes.data(), &m_total_upload_size);

				for (UInt32 i = 0; i < subresource_num; ++i) 
				{
					m_layouts[i] = { footprints[i], row_counts[i], row_sizes[i] };
				}
			}
			
			if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"テクスチャの作成に失敗しました"))
			{
				return false;
			}
			m_format = desc.format;
			m_width = desc.width;
			m_height = desc.height;

			SetState(ERHIResourceState::Common);
		}
		return true;
	}

	void D3D12Texture::Upload(void* map_ptr, const void* data)
	{
		uint8_t* dst_base = static_cast<uint8_t*>(map_ptr);
		const uint8_t* src_base = static_cast<const uint8_t*>(data);

		uint64_t current_offset = 0;

		// 全サブリソース（ミップマップ等）をループ
		for (const auto& layout : m_layouts)
		{
			uint8_t* dst_sub = dst_base + layout.footprint.Offset;
			const uint8_t* src_sub = src_base + current_offset;

			const UInt32 src_row_pitch = static_cast<UInt32>(layout.row_size_in_bytes);
			const UInt32 dst_row_pitch = layout.footprint.Footprint.RowPitch;

			// 1つのサブリソース内の全行をコピー
			for (UInt32 y = 0; y < layout.row_count; ++y)
			{
				memcpy(dst_sub + (y * dst_row_pitch), src_sub + (y * src_row_pitch), src_row_pitch);
			}

			current_offset += (static_cast<uint64_t>(src_row_pitch) * layout.row_count);
		}


	}

	void D3D12Texture::SetName(StringView name)
	{
		m_resource->SetName(name.c_str());
	}
}

#endif