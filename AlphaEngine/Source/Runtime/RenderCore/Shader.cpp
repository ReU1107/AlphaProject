#include "Shader.h"
#include "RenderHardwareInterface.h"
#include "RHI/RHIShader.h"

namespace Alpha
{
	Shader::Shader() = default;
	Shader::~Shader() = default;

	void Shader::Initialize(const Args& args)
	{
		RHIShaderDesc desc = {};
		desc.file_directory = args.file_path;
		desc.type = args.type;
		m_rhi = args.rhi->CreateShader(desc);
		m_type = args.type;
	}

	RHIShader* Shader::GetRHI() const
	{
		return m_rhi.get();
	}

	bool Shader::CheckType(ERHIShaderFlags type) const
	{
		return true;
	}
}