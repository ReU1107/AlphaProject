#include "XAudio2Effect.h"
#include "AHI/Definitions.h"
#include <xaudio2fx.h>
#include <xapofx.h>

namespace Alpha
{
	XAudio2Effect::XAudio2Effect(XAudio2Device* device)
		: AHIEffect()
		, XAudio2DeviceChild(device)
	{
	}

	XAudio2Effect::~XAudio2Effect()
	{
	}

	bool XAudio2Effect::Initialize(const AHIEffectDesc& desc)
	{
		XAudio2CreateReverb(&m_effect);

		return false;
	}
}