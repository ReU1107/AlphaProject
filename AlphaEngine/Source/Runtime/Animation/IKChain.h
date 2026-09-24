#pragma once
#include "Core/Object.h"
#include "Core/Common.h"
#include "Definitions.h"

namespace Alpha::Animation
{
	class Skeleton;

	class IKChain : public Object
	{
	private:
		Skeleton* m_skeleton{ nullptr };
		TArray<IKLink> m_links{};			// エフェクターに近い順で格納
		uint32_t m_effector_index{ 0 };		// 
		uint32_t m_target_index{ 0 };		// IKJoint(Bone)Index
		uint32_t m_max_iteration_count{ 0 };
	public:
		~IKChain() = default;

		void SetSkeleton(Skeleton* skeleton) { m_skeleton = skeleton; }

		void SetTargetIndex(uint32_t index) { m_target_index = index; }
		void SetEffectorIndex(uint32_t index) { m_effector_index = index; }
		
		void SetMaxIterationCount(uint32_t count) { m_max_iteration_count = count; }

		void AddLink(const IKLink& link) { m_links.push_back(link); }

		uint32_t GetEffectorIndex() const { return m_effector_index; }
		uint32_t GetTargetIndex() const { return m_target_index; }

		uint32_t GetMaxIterationCount() const { return m_max_iteration_count; }

		uint32_t GetLinkNum() const { return m_links.size(); }
		const IKLink& GetLink(uint32_t index) const { return m_links[index]; }
		const TArray<IKLink>& GetLinks() const { return m_links; }

		void Accept(IVisitor* visitor) final;

	};
}