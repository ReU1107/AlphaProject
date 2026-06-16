#pragma once
#include "Core/EnumClassUtil.h"

namespace Alpha
{
	enum class EBufferPresetType
	{
		// CPU更新 → GPU読み込み (Upload Heap)

		Constant,				// 定数バッファ (256Bアライメント強制, CBV用)
		Upload,					// 毎フレーム更新するバッファ

		// GPU専用 高速バッファ (Default Heap)

		ROByteAddress,			// 初期化後に固定のRawバッファ (SRV用)
		ROStructured,			// 初期化後に固定の構造化バッファ (SRV用)
		RWByteAddress,          // 読み書き可能なRawバッファ (UAV用)
		RWStructured,           // 読み書き可能な構造化バッファ (UAV用)

		// GPU書き込み → CPU読み込み (Readback Heap)

		Readback,               // GPUの計算結果（Structured/Raw問わず）をCPUに引き戻すための一時バッファ
	};
	
	enum class ESamplerPresetType
	{ 
		PointClamp, 
		LinearClamp,
		LinearWrap,
		AnisoWrap,
		Shadow,
		Count 
	};

	EnumAsIndex(ESamplerPresetType);

}