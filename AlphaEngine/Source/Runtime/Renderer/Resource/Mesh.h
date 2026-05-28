#pragma once
#include "Core/Types.h"
#include "SubMesh.h"

namespace Alpha
{
	class VertexBuffer;
	class IndexBuffer;
}

namespace Alpha::Renderer
{
	class Mesh
	{
	private:
		TUnique<VertexBuffer> m_vertex_buffer{};
		TUnique<IndexBuffer> m_index_buffer{};

		TArray<SubMesh> m_sub_meshes{};

		uint32_t m_vertex_total_count{ 0 };
		uint32_t m_index_total_count{ 0 };

	public:

		// デストラクタ
		virtual ~Mesh();

		void SetVertexTotalCount(uint32_t count) { m_vertex_total_count = count; }
		void SetIndexTotalCount(uint32_t count) { m_index_total_count = count; }

		void SetVertexBuffer(TUnique<VertexBuffer> buffer);

		void SetIndexBuffer(TUnique<IndexBuffer> buffer);

		void AddSubMesh(const SubMesh& sub_mesh) { m_sub_meshes.push_back(sub_mesh); }

		uint32_t GetVertexTotalCount() const { return m_vertex_total_count; }

		uint32_t GetIndexTotalCount() const { return m_index_total_count; }

		uint32_t GetSubMeshCount() const { return (uint32_t)m_sub_meshes.size(); }

		VertexBuffer* GetVertexBuffer() const { return m_vertex_buffer.get(); }
		IndexBuffer* GetIndexBuffer() const { return m_index_buffer.get(); }

		const TArray<SubMesh>& GetSubMeshes() const { return m_sub_meshes; }

	};
}