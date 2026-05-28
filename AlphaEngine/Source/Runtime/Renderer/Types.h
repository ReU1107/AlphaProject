#pragma once
#include "Core/EnumClassUtil.h"

namespace Alpha::Renderer
{
	enum class EMeshType
	{
		Static,
		Skeletal,
	};

	// テクスチャ合成法
	enum class BlendType
	{
		Addition,		// 加算
		Multiplication	// 乗算
	};

	enum class ETextureUsage
	{
		Albedo,
		Toon,			// 一応pmx用で持ってるけどいらなくなったら消すね↓
		SphereAdd,      //
		SphereMultiply, //
	};

	EnumAsIndex(ETextureUsage);

	// レンダーターゲット指定
	enum class ERenderTargets
	{
		None = 0,		// 
		BackBuffer = 1 << 0,
		BaseColor = 1 << 1,
		Metalic = 1 << 2,
		Specular = 1 << 3,
		Roughness = 1 << 4,
		Emissive = 1 << 5,

		Deferred = BaseColor | Metalic | Specular | Roughness | Emissive

	};

	EnumClassFlags(ERenderTargets);
}