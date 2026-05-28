#include "ModelHandle.h"
#include "Animation/Resource/Skeleton.h"

namespace Alpha
{
	ModelHandle::~ModelHandle() = default;

	Animation::Skeleton* Alpha::ModelHandle::GetSkeleton() const
	{
		return m_skeleton.get();
	}
}
