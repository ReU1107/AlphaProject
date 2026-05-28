#pragma once
#include "RenderCore/GPUResource.h"
#include "RenderCore/DescriptorHandle.h"
#include "RHI/Types.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class DescriptorHeap;
	class RHITexture;

	class Texture : public GPUResource
	{
	private:
		DescriptorHandle m_srv_handle{};
		DescriptorHandle m_uav_handle{};

	public:
		Texture();
		virtual ~Texture();
		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			DescriptorHeap* srv_heap{ nullptr };
			UInt32 width{ 256 };
			UInt32 height{ 256 };
			ERHIResourceFormat format{ ERHIResourceFormat::RGBAUnorm };
			ERHIResourceDimension dimension{ ERHIResourceDimension::Tex2D };
		};
		void Initialize(const Args& args);

		RHITexture* GetRHI() const;

		void Upload(void* map_ptr, void* data);

		uint64_t GetSize() const;

		const DescriptorHandle& GetSRVHandle() const { return m_srv_handle; }
		const DescriptorHandle& GetUAVHandle() const { return m_uav_handle; }

		UInt32 GetSRVIndex() const { return m_srv_handle.index; }
		UInt32 GetUAVIndex() const { return m_uav_handle.index; }
	};
}