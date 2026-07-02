#pragma once
#include "RenderCore/GPUResource.h"
#include "RHI/Types.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class RHITexture;

	class SwapChain;

	class Texture : public GPUResource
	{
	private:
		DescriptorHandle m_srv_handle{};
		DescriptorHandle m_uav_handle{};
		DescriptorHandle m_rtv_handle{};
		DescriptorHandle m_dsv_handle{};

	public:
		Texture();
		~Texture();
		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			DescriptorHeap* resource_heap{ nullptr };
			DescriptorHeap* rtv_heap{ nullptr };
			DescriptorHeap* dsv_heap{ nullptr };
			UInt32 width{ 256 };
			UInt32 height{ 256 };
			UInt32 depth{ 1 };
			ERHIResourceFormat format{ ERHIResourceFormat::RGBAUnorm };
			ERHIResourceDimension dimension{ ERHIResourceDimension::Tex2D };
			ERHIResourceBindFlags bind_flags{ ERHIResourceBindFlags::ShaderResource };

			// 
			SwapChain* swap_chain{ nullptr };
			UInt32 buffer_index{ 0 };
		};
		void Initialize(const Args& args);

		RHITexture* GetRHI() const;

		void Upload(void* map_ptr, const void* data);

		UInt64 GetSize() const;

		DescriptorHandle GetSRVHandle() const { return m_srv_handle; }
		DescriptorHandle GetUAVHandle() const { return m_uav_handle; }
		DescriptorHandle GetRTVHandle() const { return m_rtv_handle; }
		DescriptorHandle GetDSVHandle() const { return m_dsv_handle; }

	};
}