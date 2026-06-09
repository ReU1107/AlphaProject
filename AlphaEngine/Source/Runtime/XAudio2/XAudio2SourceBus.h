#pragma once
#include "AHI/AHISourceBus.h"
#include "XAudio2Device.h"

namespace Alpha
{
	class XAudio2SourceBus : public AHISourceBus, public XAudio2DeviceChild, public IXAudio2VoiceCallback
	{
	private:
		IXAudio2SourceVoice* m_source_voice{ nullptr };

	public:
		XAudio2SourceBus(XAudio2Device* device);
		~XAudio2SourceBus();

		bool Initialize(const AHISourceBusDesc& desc) final;


		void Start() final;
		void Pause() final;
		void Stop() final;
		void Reset() final;

		void SetVolume(float volume) final;

		// PCM供給
		void SubmitBuffer(const AHIBufferRegion& buffer) final;

		// 出力先設定
		void SetOutputs(AHISubmixBus* outputs[], UInt32 count) final;

		// エフェクト設定
		void SetEffects(AHIEffect* effects[], UInt32 count) final;

		// エフェクト有効化
		void EnableEffect(Int32 index) final;

		// エフェクト無効化
		void DisableEffect(Int32 index) final;

	private:
		// IXAudio2VoiceCallbackインターフェースのイベントハンドラ
		void __stdcall OnBufferStart(void* pBufferContext) final;
		void __stdcall OnBufferEnd(void* pBufferContext) final;
		void __stdcall OnLoopEnd(void* pBufferContext) final;
		void __stdcall OnStreamEnd() final;
		void __stdcall OnVoiceProcessingPassStart(UINT32 BytesRequired) final;
		void __stdcall OnVoiceProcessingPassEnd() final;
		void __stdcall OnVoiceError(void* pBufferContext, HRESULT Error) final;

	};
}