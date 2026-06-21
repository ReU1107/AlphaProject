#include "ModelHandle.h"
#include "Animation/Resource/Skeleton.h"

namespace Alpha
{
	ModelHandle::~ModelHandle() = default;

	void ModelHandle::SetBaseVeretices(TArray<Vector3>&& positions, TArray<Vector3>&& normals, TArray<Vector2>&& texcoords)
	{
		m_positions = std::move(positions);
		m_normals = std::move(normals);
		m_texcoords = std::move(texcoords);
		m_mesh_type = Renderer::EMeshType::Static;
	}

	void ModelHandle::SetSkeletalVertices(TArray<UVector4>&& bone_indices, TArray<Vector4>&& bone_weights, TUnique<Animation::Skeleton>&& skeleton)
	{
		m_bone_indices = std::move(bone_indices);
		m_bone_weights = std::move(bone_weights);
		m_skeleton = std::move(skeleton);

		m_mesh_type = Renderer::EMeshType::Skeletal;

	}

	void ModelHandle::SetIndices(TArray<UInt32>&& indices)
	{
		m_indices = std::move(indices);
	}

	void ModelHandle::SetMaterials(TSharedArray<MaterialHandle>&& materials)
	{
		m_materials = std::move(materials);
	}

	void ModelHandle::SetSubMeshes(TArray<SubMeshHandle>&& sub_meshes)
	{
		m_sub_meshes = std::move(sub_meshes);
	}

	Animation::Skeleton* Alpha::ModelHandle::GetSkeleton() const
	{
		return m_skeleton.get();
	}
}
