#include "XAudio2SourceBus.h"
#include "XAudio2SubmixBus.h"
#include "XAudio2Effect.h"
#include "AHI/Definitions.h"

namespace Alpha
{
	XAudio2SourceBus::XAudio2SourceBus(XAudio2Device* device)
		: AHISourceBus()
		, XAudio2DeviceChild(device)
	{
	}

	XAudio2SourceBus::~XAudio2SourceBus()
	{
		if(m_source_voice)
		{
			m_source_voice->DestroyVoice();
			m_source_voice = nullptr;
		}
	}

	bool XAudio2SourceBus::Initialize(const AHISourceBusDesc& desc)
	{
		UInt16 tag = desc.format == EAHIDigitalFormat::PCM ? 1 : 0;

		WAVEFORMATEX format = {};
		format.wFormatTag = tag;
		format.nChannels = desc.channel_count;
		format.nSamplesPerSec = desc.frequency;
		format.nBlockAlign = (desc.bit_depth / 8) * desc.channel_count;
		format.wBitsPerSample = desc.bit_depth;
		format.nAvgBytesPerSec = desc.frequency * format.nBlockAlign;

		UInt32 flags = 0;
		if (desc.use_filter)          
			flags |= XAUDIO2_VOICE_USEFILTER;
		if (!desc.pitch_shift)        
			flags |= XAUDIO2_VOICE_NOPITCH;
		if (!desc.sample_conversion)  
			flags |= XAUDIO2_VOICE_NOSRC;

		// 出力先初期化時は1つだけ
		XAUDIO2_SEND_DESCRIPTOR send_desc = {};
		XAUDIO2_VOICE_SENDS send_list = {};
		if (desc.output_target)
		{
			send_desc.Flags = 0;
			send_desc.pOutputVoice = static_cast<XAudio2SubmixBus*>(desc.output_target)->GetNativeHandle();

			send_list.SendCount = 1;
			send_list.pSends = &send_desc;
		}
		// エフェクトチェインも作成時にできるけどそっちはやらない

		auto device = GetXA2Device();

		auto hr = device->CreateSourceVoice(&m_source_voice, &format, flags, desc.max_ratio, this, (desc.output_target ? &send_list : nullptr));

		if (ALPHA_CHECK_FATAL(SUCCEEDED(hr), L"ソースボイスの作成に失敗しました"))
		{
			return false;
		}

		return true;
	}

	void XAudio2SourceBus::Start()
	{
	}

	void XAudio2SourceBus::Pause()
	{
	}

	void XAudio2SourceBus::Stop()
	{
	}

	void XAudio2SourceBus::Reset()
	{
	}

	void XAudio2SourceBus::SetVolume(float volume)
	{
		if (!m_source_voice)
			return;

		auto hr = m_source_voice->SetVolume(volume);
		if (FAILED(hr))
		{
			ALPHA_LOG_WARN(L"ソースボイスの音量設定に失敗しました");
		}
	}

	void XAudio2SourceBus::SubmitBuffer(const AHIBufferRegion& region)
	{
		if (!m_source_voice) 
			return;

		XAUDIO2_BUFFER buffer = {};
		buffer.pAudioData = static_cast<const BYTE*>(region.data);
		buffer.AudioBytes = region.sample_count; 
		buffer.PlayBegin = region.start_position;
		buffer.PlayLength = region.play_length;
		buffer.LoopBegin = region.loop_start_position;
		buffer.LoopLength = region.loop_length;
		buffer.LoopCount = region.loop_count;

		auto hr = m_source_voice->SubmitSourceBuffer(&buffer);
		if (FAILED(hr))
		{
			ALPHA_LOG_WARN(L"ソースボイスのバッファ送信に失敗しました");
		}
	}

	void XAudio2SourceBus::SetOutputs(AHISubmixBus* outputs[], UInt32 count)
	{
		XAUDIO2_SEND_DESCRIPTOR send_descs[2] = {};

		for (uint32_t i = 0; i < count; ++i)
		{
			auto xa2_output_bus = static_cast<XAudio2SubmixBus*>(outputs[i]);
			send_descs[i].pOutputVoice = xa2_output_bus->GetNativeHandle();
			send_descs[i].Flags = 0;
		}

		XAUDIO2_VOICE_SENDS send_list = {};
		send_list.SendCount = count;
		send_list.pSends = send_descs;

		m_source_voice->SetOutputVoices(&send_list);
	}

	void XAudio2SourceBus::SetEffects(AHIEffect* effects[], UInt32 count)
	{
		XAUDIO2_EFFECT_DESCRIPTOR descriptor[2] = {};
		for (uint32_t i = 0; i < count; ++i)
		{
			descriptor[i].OutputChannels = 0;
			auto xa2_effect = static_cast<XAudio2Effect*>(effects[i]);
			descriptor[i].pEffect = xa2_effect->GetNativeHandle();
		}
		
		XAUDIO2_EFFECT_CHAIN chain = {};
		chain.EffectCount = count;
		chain.pEffectDescriptors = descriptor;

		m_source_voice->SetEffectChain(&chain);
	}

	void XAudio2SourceBus::EnableEffect(Int32 index)
	{
	}

	void XAudio2SourceBus::DisableEffect(Int32 index)
	{
	}

	void __stdcall XAudio2SourceBus::OnBufferStart(void* pBufferContext)
	{
	}

	void __stdcall XAudio2SourceBus::OnBufferEnd(void* pBufferContext)
	{
	}

	void __stdcall XAudio2SourceBus::OnLoopEnd(void* pBufferContext)
	{
	}

	void __stdcall XAudio2SourceBus::OnStreamEnd()
	{
	}

	void __stdcall XAudio2SourceBus::OnVoiceProcessingPassStart(UINT32 BytesRequired)
	{
	}

	void __stdcall XAudio2SourceBus::OnVoiceProcessingPassEnd()
	{
	}

	void __stdcall XAudio2SourceBus::OnVoiceError(void* pBufferContext, HRESULT Error)
	{
	}
}
