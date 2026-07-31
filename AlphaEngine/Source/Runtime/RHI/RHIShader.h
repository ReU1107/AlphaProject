#pragma once
#include "Declarations.h"

namespace Alpha
{
	class RHIShader
	{
	public:
		virtual ~RHIShader() {}

		virtual bool Initialize(const RHIShaderDesc& desc) = 0;

		virtual const wchar_t* GetEntryPoint() const = 0;


	};
}