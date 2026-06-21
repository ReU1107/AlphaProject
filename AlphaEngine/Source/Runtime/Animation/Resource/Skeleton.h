#pragma once
#include "Core/Common.h"
#include "Core/Object.h"

namespace Alpha::Animation
{
	class Joint;
	class IKChain;
	/*
	* ボーン名とindexをマッピングさせる
	*/
	class Skeleton : public Object
	{
	private:
		TStringMap<uint32_t> m_name_map{};
		TUniqueArray<Joint> m_joints{};		// 骨群
		TUniqueArray<IKChain> m_chains{};	// 
	public:
		~Skeleton();

		// ジョイント追加
		void AddJoint(StringView name, TUnique<Joint> joint);

		void AddIKChain(TUnique<IKChain> chain);

		uint32_t GetJointNum() const { return static_cast<uint32_t>(m_joints.size()); }
		Joint* GetJoint(int32_t index) const;
		Joint* GetJoint(StringView name) const;

		int32_t FindJointIndex(StringView name) const;

		uint32_t GetIKChainNum() const { return static_cast<uint32_t>(m_chains.size()); }
		IKChain* GetIKChain(uint32_t index) const;

		void Accept(IVisitor* visitor) final;

		void Show();

	};
}