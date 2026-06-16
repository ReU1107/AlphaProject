#pragma once
#include "AssetHandle.h"
#include "Renderer/Definitions.h"

namespace Alpha
{
	namespace Animation
	{
		class Skeleton;
	}

	class MaterialHandle;
	struct SubMeshHandle
	{
		UInt32 material_index{};
		UInt32 index_offset{ 0 };
		UInt32 index_count{ 0 };
	};

	class ModelHandle : public AssetHandle
	{
	private:
		TArray<Vector3> m_positions{};
		TArray<Vector3> m_normals{};
		TArray<Vector2> m_texcoords{};
		TArray<UVector4> m_bone_indices{};
		TArray<Vector4> m_bone_weights{};


		TArray<UInt32> m_indices{};
		TSharedArray<MaterialHandle> m_materials{};
		TArray<SubMeshHandle> m_sub_meshes{};

		TUnique<Animation::Skeleton> m_skeleton{};
		Renderer::EMeshType m_mesh_type{};

	public:
		~ModelHandle();
		static AssetType Type() { return AssetType::Model; }

		AssetType GetType() const final { return AssetType::Model; }

		bool IsSkeletal() const { return (m_mesh_type == Renderer::EMeshType::Skeletal); }


		TArray<Vector3>& GetPositions() { return m_positions; }


		TArray<UInt32>& GetIndices() { return m_indices; }

		const TSharedArray<MaterialHandle>& GetMaterialHandles() const { return m_materials; }

		const TArray<SubMeshHandle>& GetSubMeshHandles() const { return m_sub_meshes; }

		Animation::Skeleton* GetSkeleton() const;

		void Accept(IVisitor* visitor) override {}

	};
}
