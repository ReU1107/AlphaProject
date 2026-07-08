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

	ERHIResourceBindFlags GPUResource::GetFlags() const
	{
		return m_rhi->GetFlags();
	}

	void GPUResource::SetName(StringView name)
	{
		m_rhi->SetName(name);
	}
}