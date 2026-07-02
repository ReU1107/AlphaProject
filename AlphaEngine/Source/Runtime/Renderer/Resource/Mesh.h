#pragma once
#include "Core/Types.h"
#include "SubMesh.h"

namespace Alpha
{
	class Buffer;
}

namespace Alpha::Renderer
{
	class Mesh
	{
	private:
		UInt32 m_;
		Buffer* m_;

		TArray<SubMesh> m_sub_meshes{};

		uint32_t m_vertex_num{ 0 };
		uint32_t m_index_num{ 0 };

	public:

		// デストラクタ
		virtual ~Mesh();

		void SetVertexNum(uint32_t count) { m_vertex_num = count; }
		void SetIndexNum(uint32_t count) { m_index_num = count; }

		void AddSubMesh(const SubMesh& sub_mesh) { m_sub_meshes.push_back(sub_mesh); }

		uint32_t GetVertexNum() const { return m_vertex_num; }

		uint32_t GetIndexNum() const { return m_index_num; }

		uint32_t GetSubMeshCount() const { return (uint32_t)m_sub_meshes.size(); }

		const TArray<SubMesh>& GetSubMeshes() const { return m_sub_meshes; }

	};
}