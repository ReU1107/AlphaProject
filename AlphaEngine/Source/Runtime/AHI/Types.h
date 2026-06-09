#pragma once

namespace Alpha
{
	enum class EAHIDigitalFormat
	{
		PCM,	// 非圧縮
		XMA,	// Xbox 360の独自圧縮形式
		ADPCM,	// 圧縮形式の一種。Xbox 360でサポートされている。
	};

	enum class EAHIEffectType
	{
		Reverb,		// リバーブ
		Echo,		// エコー
		Distortion, // ディストーション
	};
}