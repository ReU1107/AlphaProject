#include "SkeletonPose.h"
#include "Skeleton.h"
#include "Joint.h"
#include "Definitions.h"

#include "Core/Math/MathF.h"
#include <ranges>

namespace Alpha::Animation
{
	void SkeletonPose::SetSkeleton(Skeleton* source)
	{
		if (!source)
			return;

		// 
		UInt32 joint_num = source->GetJointNum();
		
		m_local_transforms.resize(joint_num);

		m_matrix_pallet.resize(joint_num);
		m_matrix_pallet.assign(joint_num, Matrix4x4::Identity);

		m_world_matrices.resize(joint_num);
		m_world_matrices.assign(joint_num, Matrix4x4::Identity);
		
		m_dirty_flags.resize(joint_num);
		m_dirty_flags.assign(joint_num, true);

		m_source = source;
		ResetToBindPose();
	}

	void SkeletonPose::SetLocalTransform(UInt32 index, const Transform& transformation)
	{
		m_local_transforms[index] = transformation;
		MarkDirty(index);
	}

	void SkeletonPose::SetLocalPosition(UInt32 index, const Vector3& location)
	{
		m_local_transforms[index].position = location;
		MarkDirty(index);
	}

	void SkeletonPose::SetLocalRotation(UInt32 index, const Quaternion& rotation)
	{
		m_local_transforms[index].quaternion = rotation;
		MarkDirty(index);
	}

	void SkeletonPose::SetLocalScale(UInt32 index, const Vector3& scale)
	{
		m_local_transforms[index].scale = scale;
		MarkDirty(index);
	}

	void SkeletonPose::ApplyPose(const Snapshot& pose)
	{
		for (auto [index, transform] : pose.bone_transforms | std::views::enumerate)
		{
			if (index >= m_source->GetJointNum())
				return;

			Transform bind_pose = m_source->GetJoint(index)->GetBindPose();

			Transform local;
			local.quaternion = bind_pose.quaternion * transform.quaternion;
			local.position = bind_pose.position + transform.position;
			local.scale = bind_pose.scale * transform.scale;

			SetLocalTransform(index, local);
		}
	}

	void SkeletonPose::ResetToBindPose()
	{
		for (UInt32 i = 0; i < m_source->GetJointNum(); ++i)
		{
			Transform bind_transform = m_source->GetJoint(i)->GetBindPose();
			SetLocalTransform(i, bind_transform);
		}
	}

	Vector3 SkeletonPose::GetWorldPosition(UInt32 index) const
	{
		Calculate(index);
		return m_world_matrices[index]._4().XYZ();
	}

	Quaternion SkeletonPose::GetWorldRotation(UInt32 index) const
	{
		Calculate(index);
		Quaternion rotation = Quaternion::Identity;
		MatrixDecompose(m_world_matrices[index], nullptr, &rotation, nullptr);
		return rotation;
	}

	Vector3 SkeletonPose::GetWorldScale(UInt32 index) const
	{
		Calculate(index);
		Vector3 scale = Vector3::One;
		MatrixDecompose(m_world_matrices[index], &scale, nullptr, nullptr);
		return scale;
	}

	const TArray<Matrix4x4>& SkeletonPose::GetMatrixPallet() const
	{
		for (auto [index, matrix] : m_matrix_pallet | std::views::enumerate)
		{
			matrix = GetProductMatrix(index);
		}

		return m_matrix_pallet;
	}

	const TArray<Matrix4x4>& SkeletonPose::GetWorldMatrices() const
	{
		for (auto [index, matrix] : m_world_matrices | std::views::enumerate)
		{
			Calculate(index);
		}
		return m_world_matrices;
	}

	Matrix4x4 SkeletonPose::GetProductMatrix(UInt32 index) const
	{
		return MatrixMultiply(m_source->GetJoint(index)->GetOffsetMatrix(), GetWorldMatrix(index));
	}

	Matrix4x4 SkeletonPose::GetWorldMatrix(UInt32 index) const
	{
		Calculate(index);
		return m_world_matrices[index];
	}

	void SkeletonPose::MarkDirty(UInt32 index)
	{
		if (m_dirty_flags[index]) 
			return;

		m_dirty_flags[index] = true;

		// 再帰的に子を再計算状態にしていく
		for (auto child : m_source->GetJoint(index)->GetChildren())
		{
			MarkDirty(child->GetIndex());
		}
	}

	void SkeletonPose::Calculate(UInt32 index) const
	{
		if (!m_dirty_flags[index])
			return;

		Matrix4x4 local_matrix = Matrix4x4Transformation(m_local_transforms[index]);

		Joint* parent = m_source->GetJoint(index)->GetParent();

		if (parent)
			m_world_matrices[index] = MatrixMultiply(local_matrix, GetWorldMatrix(parent->GetIndex()));
		else
			m_world_matrices[index] = local_matrix;

		m_dirty_flags[index] = false;
	}
}