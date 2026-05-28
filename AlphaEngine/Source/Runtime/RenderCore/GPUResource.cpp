#include "GPUResource.h"
#include "RHI/RHIResource.h"

namespace Alpha
{
	GPUResource::GPUResource() = default;
	GPUResource::~GPUResource() = default;

	RHIResource* GPUResource::GetRHI() const
	{
		return m_rhi.get();
	}
}