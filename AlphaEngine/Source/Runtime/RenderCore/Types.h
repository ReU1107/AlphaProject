#pragma once
#include "Core/EnumClassUtil.h"

namespace Alpha
{
	// 
	enum class EResourceAccessFlags
	{
		None		= 0,
		Read		= 1 << 0, // 完全読み取りだけで完結するなら
		Write		= 1 << 1, // gpu側で計算したデータを格納したりするなら
		Counter		= 1 << 2, // 一応カウンタ用だけど必要なくなったらけすかも
		CPURead		= 1 << 3, // cpu側にデータを転送するなら
		CPUWrite	= 1 << 4, // cpu側からデータを転送するなら 
		ReadOnly	= Read,
		ReadWrite	= Read | Write,
		WriteOnly	= Write,
		Append		= Read | Write | Counter,
		Consume		= Read | Write | Counter,
	};

	EnumClassFlags(EResourceAccessFlags);

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