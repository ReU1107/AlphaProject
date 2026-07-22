#pragma once
#include "AssetHandle.h"
#include "RHI/Types.h"
#include "Renderer/Types.h"

namespace Alpha
{
    class ShaderHandle : public AssetHandle
    {
    public:
 
        struct ShaderBlob
        {
            ERHIShaderType m_type{};
            String entry_point{ L"main" };
            String target_profile{ L"vs_6_8" };
            TArray<uint8_t> bytecode{};
        };

    private:

        TArray<ShaderBlob> m_stages{};

    public:
        ~ShaderHandle();

        static AssetType Type() { return AssetType::Shader; }
        AssetType GetType() const final { return AssetType::Shader; }

        //TArray<ShaderBlob>& GetStages() { return m_stages; }
        //const TArray<ShaderBlob>& GetStages() const { return m_stages; }

         void Accept(IVisitor* visitor) override {}
    };
}