#pragma once
#include "AssetHandle.h"
#include "Renderer/Definitions.h"


namespace Alpha
{
	// SkeletonHandleに変える
	namespace Animation
	{
		class Skeleton;
	}

	class MaterialTableHandle;

	// handleって名前なだけで別に管理はしていませｎ
	struct SubMeshHandle
	{
		UInt32 material_index{};
		UInt32 index_element_offset{ 0 };
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
		TShared<MaterialTableHandle> m_material_table{ nullptr };
		TArray<SubMeshHandle> m_sub_meshes{};

		TUnique<Animation::Skeleton> m_skeleton{};
		TArray<Renderer::Meshlet> m_meshlets{};
		TArray<UInt32> m_vertex_indices{};
		TArray<UInt32> m_triangle_indices{};

		ByteWriter m_vertex_attribute_serializer{};

		EMeshType m_mesh_type{};
		UInt32 m_vertex_attribute_stride{ 0 };
		EVertexAttributeFlags m_vertex_attribute_flags{};

	public:
		~ModelHandle();
		static AssetType Type() { return AssetType::Model; }

		AssetType GetType() const final { return AssetType::Model; }

		bool IsSkeletal() const { return (m_mesh_type == EMeshType::Skeletal); }

		void SetBaseVeretices(TArray<Vector3>&& positions, TArray<Vector3>&& normals, TArray<Vector2>&& texcoords);
		void SetSkeletalVertices(TArray<UVector4>&& bone_indices, TArray<Vector4>&& bone_weights, TUnique<Animation::Skeleton>&& skeleton);

		void SetIndices(TArray<UInt32>&& indices);

		void SetMaterialTable(TShared<MaterialTableHandle> table);
		void SetSubMeshes(TArray<SubMeshHandle>&& sub_meshes);
		void AddSubMesh(const SubMeshHandle& sub_mesh);

		void BuildVertexAttributeBuffer();

		const TArray<Vector3>& GetPositions() const { return m_positions; }
		const TArray<Vector3>& GetNormals() const { return m_normals; }
		const TArray<Vector2>& GetTexcoords() const { return m_texcoords; }

		const TArray<UVector4>& GetBoneIndices() const { return m_bone_indices; }
		const TArray<Vector4>& GetBoneWeights() const { return m_bone_weights; }
		const TArray<UInt32>& GetIndices() const { return m_indices; }

		const TShared<MaterialTableHandle>& GetMaterialTableHandle() const { return m_material_table; }

		const TArray<SubMeshHandle>& GetSubMeshHandles() const { return m_sub_meshes; }

		Animation::Skeleton* GetSkeleton() const;

		const TArray<Renderer::Meshlet>& GetMeshlets() const { return m_meshlets; }
		const TArray<UInt32>& GetVertexIndices() const { return m_vertex_indices; }
		const TArray<UInt32>& GetTriangleIndices() const { return m_triangle_indices; }

		const TArray<UInt8>& GetPackData() const { return m_vertex_attribute_serializer.GetData(); }
		EVertexAttributeFlags GetVertexAttributeFlags() const { return m_vertex_attribute_flags; }
		UInt32 GetVertexAttributeStride() const { return m_vertex_attribute_stride; }

		void Accept(IVisitor* visitor) override {}

	};
}
