#include "ModelHandle.h"
#include "Animation/Resource/Skeleton.h"

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
	}

	Animation::Skeleton* Alpha::ModelHandle::GetSkeleton() const
	{
		return m_skeleton.get();
	}
}
