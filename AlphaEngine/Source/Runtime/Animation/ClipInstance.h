#pragma once
#include "Core/Common.h"
#include "Core/Object.h"

namespace Alpha::Animation
{
	class Clip;
	struct Snapshot;

	class ClipInstance : public Object
	{
	public:
		virtual ~ClipInstance() = default;

		virtual void SetClip(const TShared<Clip>& clip) = 0;

		virtual void OnEvaluate(float elapsed_time, Snapshot* out_pose) = 0;

	};
}