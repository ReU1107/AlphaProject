#pragma once
#include "Core/Object.h"
#include "Core/Common.h"

namespace Alpha::Animation
{
	class ClipInstance;

	// クリップ事態は特に何かしらの処理があるわけじゃない
	class Clip : public Object
	{
	public:
		virtual ~Clip() = default;

		virtual TUnique<ClipInstance> CreateInstance() const = 0;

	};

	class TransformTrack;
	class Skeleton;

	class SkeletalClip final : public Clip
	{
	private:
		TUniqueArray<TransformTrack> m_tracks{};

	public:
		~SkeletalClip();

		TUnique<ClipInstance> CreateInstance() const final;

		void AddTrack(TUnique<TransformTrack> track);

		TransformTrack* GetTrack(uint32_t index) const;
		TransformTrack* FindTrack(StringView name) const;

		const TUniqueArray<TransformTrack>& GetTracks() const { return m_tracks; }

		void Bind(Skeleton* skeleton);

		void Accept(IVisitor* visitor) final;

	private:
		void Sort();

	};

	class MorphClip final : public Clip
	{
	public:
		~MorphClip();

		void Accept(IVisitor* visitor) override;

	};

}