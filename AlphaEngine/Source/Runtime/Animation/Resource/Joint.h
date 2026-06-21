#pragma once
#include "Core/Common.h"
#include "Core/Object.h"

namespace Alpha::Animation
{
	class Joint : public Object
	{
	private:
		Joint* m_parent{ nullptr };		// 親オブジェクト
		TArray<Joint*> m_children{};	// 子供オブジェクト配列(寿命はskeleton側が持つので生ポインタ)

		Transform m_bind_pose{};			// 初期姿勢
		Matrix4x4 m_offset_matrix{ Matrix4x4::Identity };		// 初期姿勢逆行列

		uint32_t m_index{ 0 };					// スケルトンから見た自身の配列番号

	public:
		Joint() = default;
		~Joint() = default;

		void SetIndex(uint32_t index) { m_index = index; }
		uint32_t GetIndex() const { return m_index; }

		void SetBindPose(const Transform& pose);
		void SetOffsetMatrix(const Matrix4x4& matrix);

		Transform GetBindPose() const { return m_bind_pose; }
		Matrix4x4 GetOffsetMatrix() const { return m_offset_matrix; }

		// 親ジョイント追加
		void SetParent(Joint* parent);

		// 子ジョイント追加
		void AddChild(Joint* child);

		Joint* GetParent() const { return m_parent; }

		const TArray<Joint*>& GetChildren() const { return m_children; }

		void Accept(IVisitor* visitor) final;
	};

}