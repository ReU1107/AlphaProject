#pragma once
#include "D3D12Device.h"
#include "RHI/RHITexture.h"

#include "Core/Types.h"

#if RHI_D3D12

namespace Alpha
{
	struct D3D12SubresourceLayout
	{
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint{};
		UInt32 row_count{ 0 };
		uint64_t row_size_in_bytes{ 0 };
	};


	class D3D12Texture : public RHITexture, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12Resource1> m_resource{ nullptr };
		TArray<D3D12SubresourceLayout> m_layouts{};
		

	public:
		D3D12Texture(D3D12Device* device);
		~D3D12Texture();

		bool Initialize(const RHITextureDesc& desc) final;

		void Upload(void* map_ptr, const void* data) final;

		const D3D12SubresourceLayout& GetLayout(UInt32 index) const { return m_layouts[index]; }

		void* GetNative() const final { return m_resource.Get(); }

		UInt32 GetSubresourceNum() final { return (UInt32)m_layouts.size(); }

	};

}

#endif