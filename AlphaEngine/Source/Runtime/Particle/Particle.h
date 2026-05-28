#pragma once
#include "Core/Math/Vector.h"
#include "Core/Math/Color.h"

namespace Alpha::Particle
{
	// アライメントのためにuint32_tを使用
	// パーティクルの状態
	enum class ParticleState : uint32_t
	{
		Dead = 0,	// 死んでいる
		Alive = 1,	// 生きている(前のフレームから継続的)
		Born = 2,	// 生まれたばかり
	};

	// Particle.hlsliと合わせる
	struct Unit
	{
		float3 position{ float3::Zero };	// 位置
		float life_time{ 0.0f };
		Color4 color{ Color4::Gold };

		float max_life_time{ 0.0f };

		// 一旦簡易版
		ParticleState state{ ParticleState::Dead };

		//struct SortKey
		//{
		//	float depth{ 1.0f };			// カメラからの位置
		//	uint32_t emitterID{ 0 };		// 所属エミッターの番号
		//	uint32_t particleIndex{ 0 };	// エミッター内部での番号
		//	ParticleState state{ ParticleState::Dead };	// 状態
		//}key;
	};
}