#pragma once
#include "Core/Common.h"
#include "RHI/Types.h"

namespace Alpha
{

	class RenderHardwareInterface;
	class RHIShader;

	class Shader
	{
	private:
		TUnique<RHIShader> m_rhi{ nullptr };
		ERHIShaderType m_type{ ERHIShaderType::Vertex };

	public:
		Shader();
		~Shader();
		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			const wchar_t* file_path{ nullptr };
			TArray<String> entry_points{};

			ERHIShaderType type{ ERHIShaderType::Vertex };
		};
		void Initialize(const Args& args);

		RHIShader* GetRHI() const;

		bool CheckType(ERHIShaderFlags type) const;

	};

}