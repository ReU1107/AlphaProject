#pragma once
#include "Core/Common.h"
#include "RHI/Types.h"
#include "DescriptorHandle.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class DescriptorHeap;
	class RHISamplerState;

	class SamplerState
	{
	private:
		TUnique<RHISamplerState> m_rhi{ nullptr };
		DescriptorHandle m_handle{};

	public:
		SamplerState();
		~SamplerState();
		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			DescriptorHeap* sampler_heap{ nullptr };

			ERHIFilter filter{ ERHIFilter::Linear };
			ERHIAddressMode address_u{ ERHIAddressMode::Wrap };
			ERHIAddressMode address_v{ ERHIAddressMode::Wrap };
			ERHIAddressMode address_w{ ERHIAddressMode::Wrap };

		};
		bool Initialize(const Args& args);

		void Finalize();

		DescriptorHandle GetSamplerHandle() const { return m_handle; }
	};
}