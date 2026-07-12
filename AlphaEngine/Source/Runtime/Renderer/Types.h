#pragma once
#include "Core/EnumClassUtil.h"

namespace Alpha
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

	enum class EShadingModel
	{
		Unlit		= 0, // ライティングなし（エミッシブのみ。「目」やエフェクト用）
		DefaultLit	= 1, // 通常のPBR（肌、服、髪など、ほとんどの場所用）
		Subsurface	= 2, // 肌や透ける素材用（）
		Refraction	= 3, // ガラスや水（屈折率 IOR を使うモデル）
	};

	enum class ETextureAttribute
	{
		None = 0,
		BaseColor,        // ベースカラーテクスチャ (アルベド / 不透明度)
		Normal,           // 法線テクスチャ (ノーマルマップ)
		Metallic,         // メタリックテクスチャ
		Roughness,        // ラフネステクスチャ
		Occlusion,        // アンビエントオクルージョン(AO)テクスチャ
		Emissive,         // 自発光テクスチャ

		// MMD特有の属性
		Toon,             // トゥーンテクスチャ (スナップショットシェーディング用)
		Sphere,           // スフィアテクスチャ (環境マップ/加算・乗算乗算)
	};
	using ETextureAttributeFlags = EnumFlags<ETextureAttribute>;

	// 位置を除いた頂点属性
	enum class EVertexAttribute
	{
		Normals,        // 法線 (float3)
		Tangents,       // 接線 (float3)
		Colors,         // 頂点カラー (rgba)

		// スキニング（アニメーション用）
		BoneWeights,    // ボーンのブレンドウェイト (UInt4 + float4)

		// モ-フィング（ブレンドシェイプ／表情用）
		BasePositions,  // ベース位置
		DeltaPositions, // 移動差分位置
		DeltaNormals,   // 移動差分法線
		DeltaTangents,  // 移動差分接線

		// テクスチャ座標
		Texcoords0,     // 通常のUV
		Texcoords1,     // セカンドUV（ライトマップや2枚目のマスク用）
		Texcoords2,
		Texcoords3,
	};

	using EVertexAttributeFlags = EnumFlags<EVertexAttribute>;

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