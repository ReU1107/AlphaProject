#pragma once
#include "AHI/AHISubmixBus.h"
#include "XAudio2Device.h"

namespace Alpha
{
	class XAudio2SubmixBus : public AHISubmixBus, public XAudio2DeviceChild
	{
	private:
		IXAudio2SubmixVoice* m_submix_voice{ nullptr };

	public:
		XAudio2SubmixBus(XAudio2Device* device);
		~XAudio2SubmixBus();

		bool Initialize(const AHISubmixBusDesc& desc) final;
		IXAudio2SubmixVoice* GetNativeHandle() const { return m_submix_voice; }

	};
}