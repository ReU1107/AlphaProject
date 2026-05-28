#include "Track.h"
#include "Channel.h"
#include "Skeleton.h"

namespace Alpha::Animation
{
	TransformTrack::~TransformTrack() = default;

	Transform TransformTrack::Evaluate(float time)
	{
		// skeleton側に対応するジョイント(ボーン)が存在しない
		if (m_target_index < 0)
			return Transform();

		Transform transform = {};

		if (m_translation_channel)
			transform.position = m_translation_channel->Evaluate(time);

		if (m_rotation_channel)
			transform.quaternion = m_rotation_channel->Evaluate(time);

		if (m_scale_channel)
			transform.scale = m_scale_channel->Evaluate(time);

		return transform;
	}

	void TransformTrack::BakeIndex(Skeleton* skeleton)
	{
		m_target_index = skeleton->FindJointIndex(m_target_name);
	}

	void TransformTrack::SetTranslation(TUnique<Vector3Channel> channel)
	{
		m_translation_channel = std::move(channel);
	}

	void TransformTrack::SetRotation(TUnique<QuaternionChannel> channel)
	{
		m_rotation_channel = std::move(channel);
	}

	void TransformTrack::SetScale(TUnique<Vector3Channel> channel)
	{
		m_scale_channel = std::move(channel);
	}

	void TransformTrack::Sort()
	{
		if (m_translation_channel)
			m_translation_channel->Sort();

		if (m_rotation_channel)
			m_rotation_channel->Sort();

		if (m_scale_channel)
			m_scale_channel->Sort();
	}
}