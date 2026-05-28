#pragma once
#include "ClipInstance.h"

namespace Alpha::Animation
{
	class SkeletalClip;

	class SkeletalClipInstance : public ClipInstance
	{
	private:
		TShared<SkeletalClip> m_clip{ nullptr };

	public:

		void SetClip(const TShared<Clip>& clip) final;

		void OnEvaluate(float elapsed_time, Snapshot* out_pose) final;

		void Accept(IVisitor* visitor) override;
	};
}
