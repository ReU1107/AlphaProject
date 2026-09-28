#include "Clip.h"
#include "Track.h"
#include "Channel.h"
#include "Curve.h"
#include "Skeleton.h"

#include "SkeletalClipInstance.h"

#include <algorithm>

namespace
{
	using namespace Alpha;
	using namespace Alpha::Animation;

	void PlotCurve(IVisitor* visitor, StringView label, Curve* curve)
	{
		curve->GetKeyframes();
		const int sample_count = 100;
		std::vector<float> samples(sample_count);
		float duration = curve->GetEndTime();

		//for (int i = 0; i < sample_count; ++i) {
		//	float t = (duration / (sample_count - 1)) * i;
		//	samples[i] = curve.Evaluate(t); // 補間計算ロジック
		//}


	}

}

namespace Alpha::Animation
{
	SkeletalClip::~SkeletalClip() = default;

	TUnique<ClipInstance> SkeletalClip::CreateInstance() const
	{
		return std::make_unique<SkeletalClipInstance>();
	}

	void SkeletalClip::AddTrack(TUnique<TransformTrack> track)
	{
		m_tracks.push_back(std::move(track));
	}

	TransformTrack* SkeletalClip::GetTrack(uint32_t index) const
	{
		if (index >= m_tracks.size())
			return nullptr;

		return m_tracks[index].get();
	}

	TransformTrack* SkeletalClip::FindTrack(StringView name) const
	{
		for (auto& track : m_tracks)
		{
			if (track->GetTargetName() == name)
				return track.get();
		}

		return nullptr;
	}

	void SkeletalClip::Bind(Skeleton* skeleton)
	{
		for (auto& track : m_tracks)
		{
			track->BakeIndex(skeleton);
			track->Sort();
		}
		Sort();
	}

	void SkeletalClip::Accept(IVisitor* visitor)
	{
		if (visitor->BeginTreeNode(GetName()))
		{
			for (const auto& track : m_tracks)
			{
				
				if (visitor->BeginTreeNode(track->GetTargetName().c_str()))
				{
					PlotCurve(visitor, L"Translation X", track->GetTranslation()->GetCurve(0));
					PlotCurve(visitor, L"Translation Y", track->GetTranslation()->GetCurve(1));
					PlotCurve(visitor, L"Translation Z", track->GetTranslation()->GetCurve(2));

					visitor->EndTreeNode();
				}
			}

			visitor->EndTreeNode();
		}
	}

	void SkeletalClip::Sort()
	{
		std::sort(
			m_tracks.begin(), m_tracks.end(),
			[](const TUnique<TransformTrack>& lhs, const TUnique<TransformTrack>& rhs)
			{
				int32_t lhs_index = lhs->GetJointIndex();
				int32_t rhs_index = rhs->GetJointIndex();

				// 両方が有効なインデックスなら、昇順で比較
				if (lhs_index >= 0 && rhs_index >= 0)
					return lhs_index < rhs_index;

				// 未割当のtrackは一番後ろにいくように
				return lhs_index >= 0;
			}
		);
	}

	MorphClip::~MorphClip() = default;
	void MorphClip::Accept(IVisitor* visitor)
	{
	}
}