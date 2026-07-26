#include "XAudio2MasterBus.h"

namespace Alpha
{
	// マスタリングボイス：最終出力先

	XAudio2MasterBus::XAudio2MasterBus(XAudio2Device* device)
		: AHIMasterBus()
		, XAudio2DeviceChild(device)
	{
	}

	XAudio2MasterBus::~XAudio2MasterBus()
	{
		if (m_mastering_voice)
		{
			m_mastering_voice->DestroyVoice();
			m_mastering_voice = nullptr;
		}
	}

	bool XAudio2MasterBus::Initialize(const AHIMasterBusDesc& desc)
	{
		auto device = GetXA2Device();
		auto hr = device->CreateMasteringVoice(&m_mastering_voice);
		if (ALPHA_CHECK_FATAL(SUCCEEDED(hr), L"マスタリングボイスの作成に失敗しました"))
		{
			return false;
		}

		return true;
	}

}
