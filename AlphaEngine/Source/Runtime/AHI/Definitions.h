#pragma once
#include "Core/Types.h"
#include "Types.h"
#include "Declarations.h"

namespace Alpha
{
	struct AHIMasterBusDesc
	{

	};

	struct AHISubmixBusDesc
	{

	};

	struct AHISourceBusDesc
	{
		AHISubmixBus* output_target{ nullptr };				// 出力先nullでマスタリングになる
		UInt32 frequency{ 44100 };							// サンプリング周波数 (44100HzでCD音質、48000HzでDVD音質、96000Hzでスタジオ品質)
		float max_ratio{ 1.0f };							// 再生速度の最大値(1.0fで等倍、2.0fで2倍速、0.5fで半分の速度)
		EAHIDigitalFormat format{ EAHIDigitalFormat::PCM };	// 圧縮方式
		UInt8 channel_count{ 2 };							// チャンネル数(モノラルなら 1、ステレオなら 2、サラウンドなら 6)
		UInt8 bit_depth{ 16 };								// 音の解像度
		bool use_filter{ false };							// フィルター有効か
		bool pitch_shift{ false };							// ピッチ変更有効か
		bool sample_conversion{ true };						// 周波数変換
	};

	struct AHIEffectDesc
	{
		EAHIEffectType type{ EAHIEffectType::Reverb };	//

	};

	struct AHIBufferRegion
	{
		void* data{ nullptr };              // 音声データのバッファポインタ
		UInt32 sample_count{ 0 };           // 音声データの全サイズ（バイト数、またはサンプル数）
		UInt32 start_position{ 0 };         // 最初（0）から再生
		UInt32 play_length{ 0 };            // 0 を指定すると「最後まで全再生」になる（XAudio2の仕様）
		UInt32 loop_start_position{ 0 };    // ループ開始位置（ループしないなら0）
		UInt32 loop_length{ 0 };            // 0 を指定すると「バッファ全体をループ」になる
		UInt32 loop_count{ 0 };             // 0 = ループしない（XAUDIO2_LOOP_INFINITE で無限ループ）
	};

}