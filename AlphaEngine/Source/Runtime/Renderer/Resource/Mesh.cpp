#include "Mesh.h"

#include "RenderCore/Buffer/VertexBuffer.h"
#include "RenderCore/Buffer/IndexBuffer.h"

namespace Alpha::Renderer
{
    Mesh::~Mesh() = default;

    void Mesh::SetVertexBuffer(TUnique<VertexBuffer> buffer)
    {
        m_vertex_buffer = std::move(buffer);
    }

    void Mesh::SetIndexBuffer(TUnique<IndexBuffer> buffer)
    {
        m_index_buffer = std::move(buffer);
    }
}