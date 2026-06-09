#include "XAudio2Dynamic.h"
#include "XAudio2Device.h"
#include "XAudio2MasterBus.h"
#include "XAudio2SourceBus.h"
#include "XAudio2SubmixBus.h"

namespace Alpha
{
	XAudio2Dynamic::XAudio2Dynamic()
	{
		m_device = std::make_unique<XAudio2Device>();
		m_device->Initialize();
	}

	XAudio2Dynamic::~XAudio2Dynamic()
	{
	}

	TUnique<AHIMasterBus> XAudio2Dynamic::CreateMasterBus(const AHIMasterBusDesc& desc)
	{
		return CreateAHIObject<XAudio2MasterBus>(m_device.get(), desc);
	}

	TUnique<AHISubmixBus> XAudio2Dynamic::CreateSubmixBus(const AHISubmixBusDesc& desc)
	{
		return CreateAHIObject<XAudio2SubmixBus>(m_device.get(), desc);
	}

	TUnique<AHISourceBus> XAudio2Dynamic::CreateSourceBus(const AHISourceBusDesc& desc)
	{
		return CreateAHIObject<XAudio2SourceBus>(m_device.get(), desc);
	}
}