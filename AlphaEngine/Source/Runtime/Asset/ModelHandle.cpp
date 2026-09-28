#include "ModelHandle.h"
#include "Animation/Skeleton.h"

#include <meshoptimizer/meshoptimizer.h>


/*
* 今は生データをsoaの形で持っているけど
* 独自ファイルフォーマットを作ったら完全にm_vertex_attribute_serializerだけのデータになる
*/

namespace Alpha
{
	ModelHandle::~ModelHandle() = default;

	void ModelHandle::SetBaseVeretices(TArray<Vector3>&& positions, TArray<Vector3>&& normals, TArray<Vector2>&& texcoords)
	{
		m_positions = std::move(positions);
		m_normals = std::move(normals);
		m_texcoords = std::move(texcoords);
		m_mesh_type = EMeshType::Static;
	}

	void ModelHandle::SetSkeletalVertices(TArray<UVector4>&& bone_indices, TArray<Vector4>&& bone_weights, TUnique<Animation::Skeleton>&& skeleton)
	{
		m_bone_indices = std::move(bone_indices);
		m_bone_weights = std::move(bone_weights);
		m_skeleton = std::move(skeleton);

		m_mesh_type = EMeshType::Skeletal;

	}

	void ModelHandle::SetIndices(TArray<UInt32>&& indices)
	{
		m_indices = std::move(indices);
	}

	void ModelHandle::SetMaterialTable(TShared<MaterialTableHandle> table)
	{
		m_material_table = table;
	}

	void ModelHandle::SetSubMeshes(TArray<SubMeshHandle>&& sub_meshes)
	{
		m_sub_meshes = std::move(sub_meshes);
	}

	void ModelHandle::AddSubMesh(const SubMeshHandle& sub_mesh)
	{
		m_sub_meshes.push_back(sub_mesh);
	}

	void ModelHandle::BuildVertexAttributeBuffer()
	{
		UInt32 vertex_count = m_positions.size();
		if (vertex_count == 0) 
			return;

		m_vertex_attribute_stride = 0;

		if (!m_normals.empty())
		{
			m_vertex_attribute_flags |= EVertexAttribute::Normals;
			m_vertex_attribute_stride += 12;
		}
		if (IsSkeletal())
		{
			m_vertex_attribute_flags |= EVertexAttribute::BoneWeights;
			m_vertex_attribute_stride += 32;
		}
		if (!m_texcoords.empty())
		{
			m_vertex_attribute_flags |= EVertexAttribute::Texcoords0;
			m_vertex_attribute_stride += 8;
		}

		m_vertex_attribute_serializer.Reserve(vertex_count * m_vertex_attribute_stride);

		for (UInt32 i = 0; i < vertex_count; ++i)
		{
			if (m_vertex_attribute_flags.Has(EVertexAttribute::Normals))
			{
				m_vertex_attribute_serializer.WriteVector3(m_normals[i]);
			}
			if (m_vertex_attribute_flags.Has(EVertexAttribute::BoneWeights))
			{
				m_vertex_attribute_serializer.WriteUVector4(m_bone_indices[i]);
				m_vertex_attribute_serializer.WriteVector4(m_bone_weights[i]);
			}
			if (m_vertex_attribute_flags.Has(EVertexAttribute::Texcoords0))
			{
				m_vertex_attribute_serializer.WriteVector2(m_texcoords[i]);
			}
		}


		// メッシュレットの生成
		const size_t max_vertices = 64;
		const size_t max_primitives = 126;

		// 
		const float* mesh_vertices = reinterpret_cast<const float*>(m_positions.data());
		size_t mesh_vertex_count = m_positions.size();

		for (const auto& submesh : m_sub_meshes)
		{
			// このサブメッシュが使用しているインデックス範囲だけを指すポインタを作る
			const unsigned int* submesh_indices = m_indices.data() + submesh.index_element_offset;
			size_t submesh_index_count = submesh.index_count;

			// このサブメッシュに必要な最大の Meshlet 数を計算
			size_t max_meshlets = meshopt_buildMeshletsBound(submesh_index_count, max_vertices, max_primitives);

			// 先にこれくらいつくられるかもで用意しておく
			TArray<meshopt_Meshlet> local_meshlets(max_meshlets);
			TArray<UInt32> local_vertices(max_meshlets * max_vertices);
			TArray<UInt8> local_triangles(max_meshlets * max_primitives * 3);

			// メッシュレットの構築
			size_t local_count = meshopt_buildMeshlets(
				local_meshlets.data(), local_vertices.data(), local_triangles.data(),
				submesh_indices, submesh_index_count,
				mesh_vertices, mesh_vertex_count, sizeof(Vector3),
				max_vertices, max_primitives, 0.5f
			);

			local_meshlets.resize(local_count);

			// 既に生成されている分のオフセット
			UInt32 submesh_vert_idx_offset = static_cast<UInt32>(m_vertex_indices.size());
			UInt32 submesh_tri_idx_offset = static_cast<UInt32>(m_triangle_indices.size());

			size_t valid_vertex_num = 0;
			size_t valid_triangle_num = 0;

			for (size_t i = 0; i < local_count; ++i)
			{
				const auto& opt_meshlet = local_meshlets[i];
				Renderer::Meshlet meshlet = {};
				meshlet.vertex_element_offset = submesh_vert_idx_offset + opt_meshlet.vertex_offset;
				meshlet.vertex_count = opt_meshlet.vertex_count;
				meshlet.triangle_element_offset = submesh_tri_idx_offset + (opt_meshlet.triangle_offset / 3);
				meshlet.triangle_count = opt_meshlet.triangle_count;
				meshlet.material_index = submesh.material_index;

				m_meshlets.push_back(meshlet);
				valid_vertex_num += meshlet.vertex_count;
				valid_triangle_num += meshlet.triangle_count;
			}
			// 実際に使われてる分だけに縮める
			local_vertices.resize(valid_vertex_num);

			TArray<UInt32> local_packed_triangles;
			local_packed_triangles.reserve(valid_triangle_num);

			// 有効な三角形数（1プリミティブ = 3つのインデックス）の分だけパッキング
			for (size_t i = 0; i < valid_triangle_num; ++i)
			{
				size_t tri_idx = i * 3;
				UInt32 packed = 0;
				packed |= (static_cast<UInt32>(local_triangles[tri_idx + 0] & 0xff) << 0);  // 1番目の頂点ID
				packed |= (static_cast<UInt32>(local_triangles[tri_idx + 1] & 0xff) << 8);  // 2番目の頂点ID
				packed |= (static_cast<UInt32>(local_triangles[tri_idx + 2] & 0xff) << 16); // 3番目の頂点ID

				local_packed_triangles.push_back(packed);
			}

			m_vertex_indices.insert(m_vertex_indices.end(), local_vertices.begin(), local_vertices.end());
			m_triangle_indices.insert(m_triangle_indices.end(), local_packed_triangles.begin(), local_packed_triangles.end());
		}
	}

	Animation::Skeleton* Alpha::ModelHandle::GetSkeleton() const
	{
		return m_skeleton.get();
	}
}
