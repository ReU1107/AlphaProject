#pragma once
#include "Core/Common.h"

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
		mutable TArray<UInt8>   m_dirty_flags{};      // 遅延計算用

	public:
		SkeletonPose() = default;
		~SkeletonPose() = default;

		void SetSkeleton(Skeleton* source);

		void SetLocalTransform(UInt32 index, const Transform& transformation);

		void SetLocalPosition(UInt32 index, const Vector3& location);

		void SetLocalRotation(UInt32 index, const Quaternion& rotation);

		void SetLocalScale(UInt32 index, const Vector3& scale);

		void ApplyPose(const Snapshot& pose);

		void ResetToBindPose();

		Skeleton* GetSkeleton() const { return m_source; }

		const Transform& GetTransform(UInt32 index) const { return m_local_transforms[index]; }

		const Vector3& GetLocalPosition(UInt32 index) const { return m_local_transforms[index].position; }

		const Quaternion& GetLocalRotation(UInt32 index) const { return m_local_transforms[index].quaternion; }

		const Vector3& GetLocalScale(UInt32 index) const { return m_local_transforms[index].scale; }

		Vector3 GetWorldPosition(UInt32 index) const;

		Quaternion GetWorldRotation(UInt32 index) const;

		Vector3 GetWorldScale(UInt32 index) const;

		const TArray<Matrix4x4>& GetMatrixPallet() const;
		const TArray<Matrix4x4>& GetWorldMatrices() const;

		Matrix4x4 GetProductMatrix(UInt32 index) const;

		Matrix4x4 GetWorldMatrix(UInt32 index) const;

	private:
		void MarkDirty(UInt32 index);

		void Calculate(UInt32 index) const;
	};

}