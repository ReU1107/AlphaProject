#include "Joint.h"
#include "Core/Math/MathF.h"

namespace Alpha::Animation
{
	void Joint::SetBindPose(const Transform& pose)
	{
		m_bind_pose = pose;
	}

	void Joint::SetOffsetMatrix(const Matrix4x4& matrix)
	{
		m_offset_matrix = matrix;
	}

	void Joint::SetParent(Joint* parent)
	{
		m_parent = parent;
	}

	void Joint::AddChild(Joint* child)
	{
		m_children.push_back(child);
		child->SetParent(this);
	}

	void Joint::Accept(IVisitor* visitor)
	{
		if (visitor->BeginTreeNode(GetName()))
		{
			for (auto& child : m_children)
			{
				child->Accept(visitor);
			}

			visitor->EndTreeNode();
		}
	}
}