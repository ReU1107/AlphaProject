#include "Material.h"

#include "RenderCore/Buffer/ConstantBuffer.h"
#include "RenderCore/DescriptorTable.h"

namespace Alpha::Renderer
{
    Material::~Material() = default;

    void Material::SetConstantBuffer(TUnique<ConstantBuffer> buffer)
    {
        m_constant_buffer = std::move(buffer);
    }

    void Material::SetDescriptorTable(TUnique<DescriptorTable> table)
    {
        m_texture_table = std::move(table);
    }

    const DescriptorHandle& Material::GetTextureHandle() const
    {
        return m_texture_table->GetHandle();
    }
}