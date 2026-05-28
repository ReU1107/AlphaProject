#include "IKChain.h"
#include "Skeleton.h"
#include "Joint.h"

namespace Alpha::Animation
{
	void IKChain::Accept(IVisitor* visitor)
	{
		auto joint = m_skeleton->GetJoint(m_target_index);

		if (visitor->BeginTreeNode(joint->GetName()))
		{
			String a = L"EndPoint : ";
			visitor->Text(a + m_skeleton->GetJoint(m_effector_index)->GetName());
			for (auto& link : m_links)
			{
				String tmp = L"Link : ";
				visitor->Text(tmp + m_skeleton->GetJoint(link.index)->GetName());
				tmp = L"Next : ";
				visitor->Text(tmp + m_skeleton->GetJoint(link.next_index)->GetName());

			}

			visitor->EndTreeNode();
		}

	}
}
