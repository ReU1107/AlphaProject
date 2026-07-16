#pragma once
#include "AssetHandle.h"
#include "RHI/Types.h"
#include "Renderer/Types.h"

namespace Alpha
{
	class ShaderHandle : public AssetHandle
	{
	private:
		ERHIShaderType m_type{};


	public:
		~ShaderHandle();


		static AssetType Type() { return AssetType::Shader; }
		AssetType GetType() const final { return AssetType::Shader; }



		void Accept(IVisitor* visitor) override {}


	};
}