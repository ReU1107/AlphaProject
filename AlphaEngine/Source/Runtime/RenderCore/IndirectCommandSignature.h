#pragma once
#include "Core/Types.h"
#include "RHI/Types.h"

namespace Alpha
{
    class RenderHardwareInterface;
    class RHICommandSignature;
    class RHIPipelineLayout;

    class IndirectCommandSignature
    {
    private:
        RHICommandSignature* m_rhi{ nullptr };
        UInt32 m_stride{ 0 };

    public:
        struct Args
        {
            RenderHardwareInterface* rhi{ nullptr };
            RHIPipelineLayout* layout{ nullptr };
            //RHICommandSignatureDesc desc;
        };

        bool Initialize(const Args& args);

        RHICommandSignature* GetRHI() const { return m_rhi; }
        UInt32 GetStride() const { return m_stride; }
    };

}