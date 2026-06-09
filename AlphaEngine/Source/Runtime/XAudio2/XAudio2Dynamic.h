#pragma once
#include "AHI/AHIDynamic.h"

namespace Alpha
{
	class XAudio2Device;

	class XAudio2Dynamic : public AHIDynamic
	{
	private:
		TUnique<XAudio2Device> m_device{ nullptr };

	public:
		XAudio2Dynamic();
		~XAudio2Dynamic();

		TUnique<AHIMasterBus> CreateMasterBus(const AHIMasterBusDesc& desc) final;

		TUnique<AHISubmixBus> CreateSubmixBus(const AHISubmixBusDesc& desc) final;

		TUnique<AHISourceBus> CreateSourceBus(const AHISourceBusDesc& desc) final;


	};

}