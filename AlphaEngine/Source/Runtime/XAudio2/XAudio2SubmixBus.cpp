#include "XAudio2SubmixBus.h"
namespace Alpha
{
	XAudio2SubmixBus::XAudio2SubmixBus(XAudio2Device* device)
		: AHISubmixBus()
		, XAudio2DeviceChild(device)
	{
	}

	XAudio2SubmixBus::~XAudio2SubmixBus()
	{
		if (m_submix_voice)
		{
			m_submix_voice->DestroyVoice();
			m_submix_voice = nullptr;
		}
	}

	bool XAudio2SubmixBus::Initialize(const AHISubmixBusDesc& desc)
	{
		auto device = GetXA2Device();
		auto hr = device->CreateSubmixVoice(&m_submix_voice, 0, 0);
		if (ALPHA_CHECK_FATAL(SUCCEEDED(hr), L"サブミックスボイスの作成に失敗しました"))
		{
			return false;
		}

		return true;
	}
}