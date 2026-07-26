#include "XAudio2Device.h"


#pragma comment(lib,"xaudio2.lib")

namespace Alpha
{
    XAudio2Device::~XAudio2Device()
    {
        if (m_device)
        {
            m_device->Release();
            m_device = nullptr;
        }

        CoUninitialize();
    }

    bool XAudio2Device::Initialize()
	{
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        if (ALPHA_CHECK_FATAL(SUCCEEDED(hr), L"COMの初期化に失敗しました"))
        {
            return false;
        }

        UInt32 flag = 0;

        // デバイス作成
        hr = XAudio2Create(&m_device, flag);


        if (ALPHA_CHECK_FATAL(SUCCEEDED(hr), L"XAudio2デバイスの作成に失敗しました"))
        {
            CoUninitialize();
            return false;
        }

#if _DEBUG
        XAUDIO2_DEBUG_CONFIGURATION debug = {};
        debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
        debug.BreakMask = XAUDIO2_LOG_ERRORS;
        m_device->SetDebugConfiguration(&debug, nullptr);
#endif
		return true;
	}
}