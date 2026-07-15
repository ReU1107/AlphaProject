#pragma once
#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/Quaternion.h"

namespace Alpha
{
	struct TransformComponent
	{
		Matrix4x4 local_to_world{ Matrix4x4::Identity };// ローカル→ワールド変換行列
		Matrix4x4 world_to_local{ Matrix4x4::Identity };// ワールド→ローカル変換行列

		Quaternion rotation{ Quaternion::Identity };	// 回転(四元数)

		Vector3 position{ Vector3::Zero };				// 位置		親からみた相対的な位置
		Vector3 scale{ Vector3::One };					// スケーリング

		// ダーティパターン
		bool is_dirty;									// 再計算(ワールド変換行列作成に必要な数値が変わったらtrue)

	};
}