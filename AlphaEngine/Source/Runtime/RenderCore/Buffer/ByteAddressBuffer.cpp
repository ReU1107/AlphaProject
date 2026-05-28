#include "ByteAddressBuffer.h"
#include "RenderCore/RenderHardwareInterface.h"
#include "RHI/Definitions.h"
#include "RHI/RHIBuffer.h"

namespace Alpha
{
	ByteAddressBuffer::ByteAddressBuffer() = default;
	ByteAddressBuffer::~ByteAddressBuffer() = default;

	void ByteAddressBuffer::Initialize(const Args& args)
	{
		RHIBufferDesc buffer_desc = {};
		buffer_desc.stride = args.base.stride;
		buffer_desc.size = args.base.element_count * args.base.stride;
		buffer_desc.access = ERHIBufferAccess::Static;
		buffer_desc.flags |= ERHIBufferBindFlags::Raw;

		if (EnumHasFlag(args.access_flags, EResourceAccessFlags::Write))
			buffer_desc.flags |= ERHIBufferBindFlags::UnorderedAccess;

		m_rhi = args.base.rhi->CreateBuffer(buffer_desc);

	}

}