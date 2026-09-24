#include "Skeleton.h"
#include "Joint.h"
#include "IKChain.h"

namespace Alpha::Animation
{
	Skeleton::~Skeleton() = default;

	void Skeleton::AddJoint(StringView name, TUnique<Joint> joint)
	{
		if (m_name_map.contains(name))
			return;
		auto ptr = joint.get();
		m_joints.push_back(std::move(joint));
		uint32_t index = static_cast<uint32_t>(m_joints.size() - 1);

		ptr->SetIndex(index);
		ptr->SetName(name);

		m_name_map.emplace(name, index);
	}

	void Skeleton::AddIKChain(TUnique<IKChain> chain)
	{
		chain->SetSkeleton(this);
		m_chains.push_back(std::move(chain));
	}

	Joint* Skeleton::GetJoint(int32_t index) const
	{
		if (index < 0 || index >= static_cast<int32_t>(m_joints.size()))
			return nullptr;
		return m_joints[index].get();
	}

	Joint* Skeleton::GetJoint(StringView name) const
	{
		int32_t index = FindJointIndex(name);
		if (index < 0) 
			return nullptr;
		return m_joints[index].get();
	}

	int32_t Skeleton::FindJointIndex(StringView name) const
	{
		auto found = m_name_map.find(name);
		if (found == m_name_map.cend())
			return -1;

		return static_cast<int32_t>(found->second);
	}

	IKChain* Skeleton::GetIKChain(uint32_t index) const
	{
		if (index >= static_cast<uint32_t>(m_chains.size()))
			return nullptr;
		return m_chains[index].get();
	}

	void Skeleton::Accept(IVisitor* visitor)
	{
		if (visitor->BeginTreeNode(L"Skeleton"))
		{
			if (visitor->BeginTreeNode(L"Bones"))
			{
				for (auto& joint : m_joints)
				{
					if (!joint->GetParent())
						joint->Accept(visitor);
				}

				visitor->EndTreeNode();
			}
			if (visitor->BeginTreeNode(L"IKChain"))
			{
				for (auto& chain : m_chains)
				{
					chain->Accept(visitor);
				}

				visitor->EndTreeNode();
			}


			visitor->EndTreeNode();
		}
	}

	void Skeleton::Show()
	{

	}
}