#pragma once
#include "AssetHandle.h"

namespace Alpha
{
	namespace Animation
	{
		class SkeletalClip;
		class MorphClip;
	}

	class MotionHandle : public AssetHandle
	{
	private:
		TShared<Animation::SkeletalClip> m_skeletal{};
		TShared<Animation::MorphClip> m_morph{};
		friend class VMDLoader;
	public:
		~MotionHandle();

		static AssetType Type() { return AssetType::Motion; }

		AssetType GetType() const final { return AssetType::Motion; }

		TShared<Animation::SkeletalClip> GetSkeletalClip() const { return m_skeletal; }
		void Accept(IVisitor* visitor) override {}

	};

}
