#pragma once
#include "AHI/AHIMasterBus.h"
#include "XAudio2Device.h"

namespace Alpha
{
	class XAudio2MasterBus : public AHIMasterBus , public XAudio2DeviceChild
	{
	private:
		IXAudio2MasteringVoice* m_mastering_voice{ nullptr };

	public:
		XAudio2MasterBus(XAudio2Device* device);
		~XAudio2MasterBus();

		bool Initialize(const AHIMasterBusDesc& desc) final;

		//virtual void SetVolume(float volume) override;
		//virtual void SetMute(bool mute) override;
	};
}