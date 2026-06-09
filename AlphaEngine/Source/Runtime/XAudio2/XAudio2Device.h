#pragma once

#include <xaudio2.h>
#include "Core/Common.h"

namespace Alpha
{
	class XAudio2Device
	{
	private:
		ComPtr<IXAudio2> m_device{ nullptr };
	public:

		~XAudio2Device();

		bool Initialize();

		IXAudio2* GetDevice() const { return m_device.Get(); }
	};


	class XAudio2DeviceChild
	{
	public:
		XAudio2DeviceChild(XAudio2Device* device)
			: m_device(device)
		{

		}
		virtual ~XAudio2DeviceChild()
		{
			m_device = nullptr;
		}

	protected:
		XAudio2Device* GetDeviceObject() const { return m_device; }
		IXAudio2* GetXA2Device() const { return m_device->GetDevice(); }


	private:
		XAudio2Device* m_device{ nullptr };
	};
}