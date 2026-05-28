#include "SkeletalClipInstance.h"
#include "Snapshot.h"
#include "Animation/Resource/Clip.h"
#include "Animation/Resource/Track.h"

namespace Alpha::Animation
{
	void SkeletalClipInstance::SetClip(const TShared<Clip>& clip)
	{
		m_clip = std::dynamic_pointer_cast<SkeletalClip>(clip);

		//Assert(m_clip != nullptr && "Passed clip is not a SkeletalClip!");
	}

	void SkeletalClipInstance::OnEvaluate(float elapsed_time, Snapshot* out_pose)
	{
		for (auto& track : m_clip->GetTracks())
		{
			auto index = track->GetJointIndex();
			if (index < 0)
				continue;
			out_pose->bone_transforms[index] = track->Evaluate(elapsed_time);
		}
	}

	void SkeletalClipInstance::Accept(IVisitor* visitor)
	{

	}
}