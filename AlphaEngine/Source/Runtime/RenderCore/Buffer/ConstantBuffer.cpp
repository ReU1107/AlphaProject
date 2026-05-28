#include "ConstantBuffer.h"
#include "RenderCore/RenderHardwareInterface.h"
#include "RHI/Definitions.h"
#include "RHI/Types.h"
#include "RHI/RHIBuffer.h"

namespace Alpha
{
	
	ConstantBuffer::~ConstantBuffer() = default;

	void ConstantBuffer::Initialize(const Args& args)
	{
		RHIBufferDesc buffer_desc = {};
		buffer_desc.stride = args.base.stride;
		buffer_desc.size = args.base.element_count * args.base.stride;
		buffer_desc.access = ERHIBufferAccess::Dynamic;

		m_rhi = args.base.rhi->CreateBuffer(buffer_desc);
	}

	void ConstantBuffer::WriteData(void* data)
	{
		GetRHI()->Write(data);
	}

}