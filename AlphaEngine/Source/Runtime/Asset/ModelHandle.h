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
		uint32_t material_index{};
		uint32_t index_offset{ 0 };
		uint32_t index_count{ 0 };
	};

	class ModelHandle : public AssetHandle
	{
	private:
		TArray<Renderer::StaticVertex> m_static_vertices{};
		TArray<Renderer::SkeletalVertex> m_skeletal_vertices{};

		TArray<uint32_t> m_indices{};
		TSharedArray<MaterialHandle> m_materials{};
		TArray<SubMeshHandle> m_sub_meshes{};

		TUnique<Animation::Skeleton> m_skeleton{};
		Renderer::EMeshType m_mesh_type{};

		friend class PMXLoader;
	public:
		~ModelHandle();
		static AssetType Type() { return AssetType::Model; }

		AssetType GetType() const final { return AssetType::Model; }

		bool IsSkeletal() const { return (m_mesh_type == Renderer::EMeshType::Skeletal); }

		TArray<Renderer::StaticVertex>& GetStaticVeretices() { return m_static_vertices; }
		TArray<Renderer::SkeletalVertex>& GetSkeletalVeretices() { return m_skeletal_vertices; }

		TArray<uint32_t>& GetIndices() { return m_indices; }

		const TSharedArray<MaterialHandle>& GetMaterialHandles() const { return m_materials; }

		const TArray<SubMeshHandle>& GetSubMeshHandles() const { return m_sub_meshes; }

		Animation::Skeleton* GetSkeleton() const;

		void Accept(IVisitor* visitor) override {}

	};
}
