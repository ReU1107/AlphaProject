#include "StructuredBuffer.h"
#include "RenderCore/RenderHardwareInterface.h"
#include "RHI/Definitions.h"
#include "RHI/Types.h"
#include "RHI/RHIBuffer.h"

namespace Alpha
{
	StructuredBuffer::StructuredBuffer() = default;
	StructuredBuffer::~StructuredBuffer() = default;

	void StructuredBuffer::Initialize(const Args& args)
	{
		RHIBufferDesc buffer_desc = {};
		buffer_desc.stride = args.base.stride;
		buffer_desc.size = args.base.element_count * args.base.stride;

		if (EnumHasFlag(args.access_flags, EResourceAccessFlags::CPURead))
			buffer_desc.access = ERHIBufferAccess::Staging;
		else if (EnumHasFlag(args.access_flags, EResourceAccessFlags::CPUWrite))
			buffer_desc.access = ERHIBufferAccess::Dynamic;
		else
			buffer_desc.access = ERHIBufferAccess::Static;

		if(EnumHasFlag(args.access_flags, EResourceAccessFlags::Write))
			buffer_desc.flags |= ERHIBufferBindFlags::UnorderedAccess;

		m_rhi = args.base.rhi->CreateBuffer(buffer_desc);
	}
}