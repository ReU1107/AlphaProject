#pragma once
#include "Core/Types.h"

namespace Alpha::Animation
{
	struct Snapshot;

	class Skeleton;

	class SkeletonPose
	{
	private:
		Skeleton* m_source{ nullptr };
		mutable TArray<Matrix4x4> m_matrix_pallet{};	// 行列パレット
		mutable TArray<Transform> m_local_transforms{}; // 現在のポーズ
		mutable TArray<Matrix4x4> m_world_matrices{};   // 計算済みの行列
		mutable TArray<uint8_t>   m_dirty_flags{};      // 遅延計算用

	public:
		SkeletonPose() = default;
		~SkeletonPose() = default;

		void SetSkeleton(Skeleton* source);

		void SetLocalTransform(uint32_t index, const Transform& transformation);

		void SetLocalPosition(uint32_t index, const Vector3& location);

		void SetLocalRotation(uint32_t index, const Quaternion& rotation);

		void SetLocalScale(uint32_t index, const Vector3& scale);

		void ApplyPose(const Snapshot& pose);

		void ResetToBindPose();

		Skeleton* GetSkeleton() const { return m_source; }

		const Transform& GetTransform(uint32_t index) const { return m_local_transforms[index]; }

		const Vector3& GetLocalPosition(uint32_t index) const { return m_local_transforms[index].position; }

		const Quaternion& GetLocalRotation(uint32_t index) const { return m_local_transforms[index].quaternion; }

		const Vector3& GetLocalScale(uint32_t index) const { return m_local_transforms[index].scale; }

		Vector3 GetWorldPosition(uint32_t index) const;

		Quaternion GetWorldRotation(uint32_t index) const;

		Vector3 GetWorldScale(uint32_t index) const;

		const TArray<Matrix4x4>& GetMatrixPallet() const;
		const TArray<Matrix4x4>& GetWorldMatrices() const;

		Matrix4x4 GetProductMatrix(uint32_t index) const;

		Matrix4x4 GetWorldMatrix(uint32_t index) const;

	private:
		void MarkDirty(uint32_t index);

		void Calculate(uint32_t index) const;
	};

}