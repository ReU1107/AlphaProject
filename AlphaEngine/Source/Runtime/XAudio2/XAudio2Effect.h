#pragma once
#include "AHI/AHIEffect.h"
#include "XAudio2Device.h"

#include <xapo.h>

namespace Alpha
{
	class XAudio2Effect : public AHIEffect, public XAudio2DeviceChild
	{
	private:
		IUnknown* m_effect{ nullptr };
	public:
		XAudio2Effect(XAudio2Device* device);
		~XAudio2Effect();
		bool Initialize(const AHIEffectDesc& desc) final;

		IUnknown* GetNativeHandle() const { return m_effect; }
	};
}
