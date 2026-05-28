#pragma once
#include "Core/Types.h"

namespace Alpha::Animation
{
	class Track
	{
	private:

	public:
		Track() {}
		virtual ~Track() {}

		virtual void Sort() = 0;

	};

	class Vector3Channel;
	class QuaternionChannel;
	class Skeleton;

	class TransformTrack : public Track
	{
	private:
		TUnique<Vector3Channel>	m_translation_channel{ nullptr };
		TUnique<QuaternionChannel> m_rotation_channel{ nullptr };
		TUnique<Vector3Channel>	m_scale_channel{ nullptr };
		String m_target_name{};
		int32_t m_target_index{ -1 };

	public:
		~TransformTrack();

		Transform Evaluate(float time);

		void BakeIndex(Skeleton* skeleton);

		void SetTranslation(TUnique<Vector3Channel> channel);
		void SetRotation(TUnique<QuaternionChannel> channel);
		void SetScale(TUnique<Vector3Channel> channel);

		Vector3Channel* GetTranslation() const { return m_translation_channel.get(); }
		QuaternionChannel* GetRotation() const { return m_rotation_channel.get(); }
		Vector3Channel* GetScale() const { return m_scale_channel.get(); }

		void SetTargetName(StringView name) { m_target_name = name; }
		
		const String& GetTargetName() const { return m_target_name; }

		int32_t GetJointIndex() const { return m_target_index; }

		void Sort() final;
	};

}