#pragma once
#include "Core/Types.h"
#include "Declarations.h"

namespace Alpha
{

	class AHISourceBus
	{
	public:
		virtual ~AHISourceBus() {};

		virtual bool Initialize(const AHISourceBusDesc& desc) = 0;

		// 再生開始
		virtual void Start() = 0;
		// 再生一時停止
		virtual void Pause() = 0;
		// 再生完全停止
		virtual void Stop() = 0;

		virtual void Reset() = 0;

		virtual void SetVolume(float volume) = 0;

		// PCM供給
		virtual void SubmitBuffer(const AHIBufferRegion& buffer) = 0;

		// 出力先設定
		virtual void SetOutputs(AHISubmixBus* outputs[], UInt32 count) = 0;

		// エフェクト設定
		virtual void SetEffects(AHIEffect* effects[], UInt32 count) = 0;

		// エフェクト有効化
		virtual void EnableEffect(Int32 index) = 0;

		// エフェクト無効化
		virtual void DisableEffect(Int32 index) = 0;

	};
}