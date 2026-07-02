#include "ModelHandle.h"
#include "Animation/Resource/Skeleton.h"

#include <meshoptimizer/meshoptimizer.h>

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

    void ModelHandle::Optimize()
    {

        const size_t max_vertices = 64;
        const size_t max_primitives = 126;

        for (const auto& submesh : GetSubMeshHandles())
        {
            // このサブメッシュが使用しているインデックス範囲だけを指すポインタを作る
            const unsigned int* submesh_indices = m_indices.data() + submesh.index_offset;
            size_t submesh_index_count = submesh.index_count;

            // 1. このサブメッシュに必要な最大の Meshlet 数を計算
            size_t max_meshlets = meshopt_buildMeshletsBound(submesh_index_count, max_vertices, max_primitives);

            std::vector<meshopt_Meshlet> local_meshlets(max_meshlets);
            std::vector<unsigned int>    local_vertices(max_meshlets * max_vertices);
            std::vector<unsigned char>   local_triangles(max_meshlets * max_primitives * 3);

            // 2. このサブメッシュだけで Meshlet をビルド（境界が混ざらなくなる）
            size_t local_count = meshopt_buildMeshlets(
                local_meshlets.data(), local_vertices.data(), local_triangles.data(),
                submesh_indices, submesh_index_count,
                &m_positions[0].X(), m_positions.size(), sizeof(Vector3),
                max_vertices, max_primitives, 0.5f
            );

            local_meshlets.resize(local_count);

            // 3. 【最重要】グローバル配列に追加する際、オフセットを調整する
            // meshoptimizer が出力する offset は、この submesh 内での「ローカルオフセット」なので、
            // 全体バッファ内での正しい位置を指すように、現在のグローバル配列のサイズ分だけ下駄を履かせます。
            unsigned int vert_offset_bias = static_cast<unsigned int>(m_global_meshlet_vertices.size());
            unsigned int tri_offset_bias = static_cast<unsigned int>(m_global_meshlet_triangles.size());

            for (auto& m : local_meshlets)
            {
                m.vertex_offset += vert_offset_bias;
                m.triangle_offset += tri_offset_bias;

                // ★必要であれば、HLSL側でマテリアルを切り替えるためのIDをここに仕込むことも可能です
                // m.ambient_radius = submesh.MaterialID; // 本来の使い方ではないが、空き変数（float）を無理矢理使うか、独自構造体を作る
            }

            // 実際のデータを取得した分だけ縮小
            if (!local_meshlets.empty()) 
            {
                const auto& last = local_meshlets.back();
                local_vertices.resize(last.vertex_offset - vert_offset_bias + last.vertex_count);
                local_triangles.resize(last.triangle_offset - tri_offset_bias + ((last.triangle_count * 3 + 3) & ~3));
            }

            // 4. グローバル配列へ結合
            m_global_meshlets.insert(m_global_meshlets.end(), local_meshlets.begin(), local_meshlets.end());
            m_global_meshlet_vertices.insert(m_global_meshlet_vertices.end(), local_vertices.begin(), local_vertices.end());
            m_global_meshlet_triangles.insert(m_global_meshlet_triangles.end(), local_triangles.begin(), local_triangles.end());
        }


    }

	Animation::Skeleton* Alpha::ModelHandle::GetSkeleton() const
	{
		return m_skeleton.get();
	}
}
