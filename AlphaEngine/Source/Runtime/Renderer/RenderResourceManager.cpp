#include "RenderResourceManager.h"
#include "Asset/ModelHandle.h"
#include "Asset/MaterialHandle.h"
#include "Asset/TextureHandle.h"
#include "Asset/AssetManager.h"

#include "RenderCore/Module.h"

namespace Alpha
{
    using namespace Renderer;
	RenderResourceManager::~RenderResourceManager() = default;

	bool RenderResourceManager::Initialize(const Args& args)
	{
		auto CreateAllocator = [](size_t alignment, size_t max_size, EBufferPresetType preset, const wchar_t* name) 
		{
			auto allocator = std::make_unique<PersistentBufferAllocator>();
			allocator->Initialize({
				.render_core = RenderCore::GetPtr(),
				.alignment = alignment,
				.max_size = max_size,
				.preset = preset,
				});
			allocator->GetBuffer()->SetName(name);
			return allocator;
		};

		m_vertex_position_allocator	= CreateAllocator(sizeof(Vector3), 12_MB, EBufferPresetType::ROStructured, L"VertexPositionBuffer");
		m_vertex_attribute_allocator= CreateAllocator(sizeof(UInt32), 12_MB, EBufferPresetType::ROByteAddress, L"VertexAttributeBuffer");
		
        m_vertex_indices_allocator	= CreateAllocator(sizeof(UInt32), 12_MB, EBufferPresetType::ROStructured, L"VertexIndicesBuffer");
		m_triangle_indices_allocator= CreateAllocator(sizeof(UInt32), 12_MB, EBufferPresetType::ROStructured, L"TriangleIndicesBuffer");
		m_meshlet_allocator			= CreateAllocator(sizeof(Meshlet), 12_MB, EBufferPresetType::ROStructured, L"MeshletBuffer");
		
        m_mesh_allocator			= CreateAllocator(sizeof(MeshData), 4_MB, EBufferPresetType::ROStructured, L"ModelBuffer");
		
        m_material_allocator		= CreateAllocator(sizeof(MaterialData), 4_MB, EBufferPresetType::ROStructured, L"MaterialBuffer");

        m_material_table_allocator  = CreateAllocator(sizeof(UInt32), 4_MB, EBufferPresetType::ROByteAddress, L"MaterialTableBuffer");

        m_mesh_palette.Initialize(100);
        m_material_palette.Initialize(100);
        m_texture_palette.Initialize(100);

		return true;
	}

	void RenderResourceManager::Finalize()
	{

	}

	UInt32 RenderResourceManager::RegisterMesh(TShared<ModelHandle> model)
	{
        Mesh& mesh = m_mesh_palette.Allocate(model->GetID());

        {
			UInt32 num = model->GetPositions().size();
			Allocation alloc{};
			if (!m_vertex_position_allocator->AllocateByNum(num, &alloc))
			{
				ALPHA_LOG_WARN(L"ModelHandleのリソース化に失敗しました");
				// 
			}
			
			RenderCore::Get().UploadBuffer(m_vertex_position_allocator->GetBuffer(), alloc.offset, model->GetPositions().data(), sizeof(Vector3) * num);

            mesh.vertex_position_alloc = alloc;
		}

		{
			UInt32 size = model->GetPackData().size();
			Allocation alloc{};
			if (!m_vertex_attribute_allocator->AllocateBySize(size, &alloc))
			{
				ALPHA_LOG_WARN(L"ModelHandleのリソース化に失敗しました");
				// 
			}
			RenderCore::Get().UploadBuffer(m_vertex_attribute_allocator->GetBuffer(), alloc.offset, model->GetPackData().data(), size);

            mesh.vertex_attribute_alloc = alloc;
		}

        {
            mesh.data.vertex_position_offset = mesh.vertex_position_alloc.offset / sizeof(Vector3);
            mesh.data.vertex_attribute_offset = mesh.vertex_attribute_alloc.offset;  // ByteAddressBufferだからそのまま
            mesh.data.vertex_stride = model->GetVertexAttributeStride();
            mesh.data.attribute_flags = model->GetVertexAttributeFlags().GetValue();

            Allocation alloc{};
            m_mesh_allocator->AllocateByNum(1, &alloc);

            RenderCore::Get().UploadBuffer(m_mesh_allocator->GetBuffer(), alloc.offset, &mesh.data, sizeof(MeshData));

            mesh.data_alloc = alloc;
        }

        const size_t max_vertices = 64;
        const size_t max_primitives = 126;

        // 
        const float* mesh_vertices = model->GetPositions()[0].data();
        size_t mesh_vertex_count = model->GetPositions().size();

        for (const auto& submesh : model->GetSubMeshHandles())
        {
            // このサブメッシュが使用しているインデックス範囲だけを指すポインタを作る
            const unsigned int* submesh_indices = model->GetIndices().data() + submesh.index_offset;
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
            UInt32 submesh_vert_idx_offset = static_cast<UInt32>(mesh.vertex_indices.size());
            UInt32 submesh_tri_idx_offset = static_cast<UInt32>(mesh.triangle_indices.size());

            size_t valid_vertex_num = 0;
            size_t valid_triangle_num = 0;

            for (size_t i = 0; i < local_count; ++i)
            {
                const auto& opt_meshlet = local_meshlets[i];
                Meshlet meshlet = {};
                meshlet.vertex_offset = submesh_vert_idx_offset + opt_meshlet.vertex_offset;
                meshlet.vertex_count = opt_meshlet.vertex_count;
                meshlet.triangle_offset = submesh_tri_idx_offset + (opt_meshlet.triangle_offset / 3);
                meshlet.triangle_count = opt_meshlet.triangle_count;
                meshlet.material_index = submesh.material_index;
                meshlet.mesh_index = mesh.data_alloc.offset / sizeof(MeshData);

                mesh.meshlets.push_back(meshlet);
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

            mesh.vertex_indices.insert(mesh.vertex_indices.end(), local_vertices.begin(), local_vertices.end());
            mesh.triangle_indices.insert(mesh.triangle_indices.end(), local_packed_triangles.begin(), local_packed_triangles.end());
        }

        {
            /*
            * m_vertex_indicesの中身に対してglobal_vertex_offsetを加算した値が全体の頂点バッファのオフセットになる
            */
            UInt32 num = mesh.vertex_indices.size();
            Allocation alloc = {};
            m_vertex_indices_allocator->AllocateByNum(num, &alloc);
            RenderCore::Get().UploadBuffer(m_vertex_indices_allocator->GetBuffer(), alloc.offset, mesh.vertex_indices.data(), sizeof(UInt32) * num);
            
            /*
            * meshletのoffsetは頂点バッファのオフセットではなく、頂点インデックスバッファのオフセットを指す必要がある
            * そのためこのoffsetをmeshletのvertex_offsetに加算して設定する必要がある
            */
            mesh.vertex_inidices_alloc = alloc;

        }

        {
            UInt32 num = mesh.triangle_indices.size();
            Allocation alloc = {};
            m_triangle_indices_allocator->AllocateByNum(num, &alloc);
            RenderCore::Get().UploadBuffer(m_triangle_indices_allocator->GetBuffer(), alloc.offset, mesh.triangle_indices.data(), sizeof(UInt32) * num);
            /*
            * meshletのtriangle_offsetは三角形インデックスバッファのオフセットを指す必要がある
            * そのためこのoffsetをmeshletのtriangle_offsetに加算して設定する必要がある
            */
            mesh.triangle_indices_alloc = alloc;
        }

        {
            UInt64 vertex_offset = mesh.vertex_inidices_alloc.offset / sizeof(UInt32);
            UInt64 triangle_offset = mesh.triangle_indices_alloc.offset / sizeof(UInt32);

            for (auto& meshlet : mesh.meshlets)
            {
                meshlet.vertex_offset += static_cast<UInt32>(vertex_offset);
                meshlet.triangle_offset += static_cast<UInt32>(triangle_offset);
            }
            Allocation alloc = {};
            UInt32 num = mesh.meshlets.size();
            m_meshlet_allocator->AllocateByNum(num, &alloc);
            RenderCore::Get().UploadBuffer(m_meshlet_allocator->GetBuffer(), alloc.offset, mesh.meshlets.data(), sizeof(Meshlet) * num);

            mesh.meshlet_alloc = alloc;
        }

        // マテリアルの登録
        for (auto& material_handle : model->GetMaterialHandles())
        {
            RegisterMaterial(material_handle);
        }

        return 0;
	}

    void RenderResourceManager::RegisterMaterial(TShared<MaterialHandle> handle)
    {
        Material& material = m_material_palette.Allocate(handle->GetID());

        // このマテリアルが利用するテクスチャのsrvindex
        TMap<ETextureAttribute, UInt32> local_tex_cache;

        for (auto& texture_handle : handle->GetTextureHandles())
        {
            ETextureAttribute attr = texture_handle->GetAttribute();
            if (attr == ETextureAttribute::None) 
                continue;

            local_tex_cache[attr] = RegisterTexture(texture_handle);
        }

        auto white_tex = RegisterTexture(AssetManager::Get().GetWhiteTexture());
        auto black_tex = RegisterTexture(AssetManager::Get().GetBlackTexture());
        auto normal_tex = RegisterTexture(AssetManager::Get().GetNormalTexture());

        auto GetTexture = [&](ETextureAttribute attr, UInt32 fallback_srv)
        {
            auto found = local_tex_cache.find(attr);
            return (found != local_tex_cache.cend()) ? found->second : fallback_srv;
        };

        material.data.props = handle->GetProperties();

        material.data.base_color_tex_idx = GetTexture(ETextureAttribute::BaseColor, white_tex);
        material.data.normal_tex_idx = GetTexture(ETextureAttribute::Normal, normal_tex);
        material.data.metallic_roughness_tex_idx = GetTexture(ETextureAttribute::Metallic, white_tex);
        material.data.emissive_tex_idx = GetTexture(ETextureAttribute::Emissive, black_tex);
        material.data.occlusion_tex_idx = GetTexture(ETextureAttribute::Occlusion, white_tex);
        material.data.toon_tex_idx = GetTexture(ETextureAttribute::Toon, white_tex);
        material.data.sphere_tex_idx = GetTexture(ETextureAttribute::Sphere, white_tex);

        // MaterialTable方式をとるため1つずつ登録していく別に連続してなくても問題ない
        Allocation alloc{};
        m_material_allocator->AllocateByNum(1, &alloc);

        RenderCore::Get().UploadBuffer(m_material_allocator->GetBuffer(), alloc.offset, &material.data, sizeof(MaterialData));

        material.alloc = alloc;
    }

    UInt32 RenderResourceManager::RegisterTexture(TShared<TextureHandle> handle)
    {
        TextureResource& texture = m_texture_palette.Allocate(handle->GetID());

        if (texture.tex)
            return texture.tex.get()->GetSRVHandle().index;

        auto tex = RenderCore::Get().CreateTexture(
            handle->GetWidth(),
            handle->GetHeight(),
            handle->GetBindFlags(),
            handle->GetFormat(),
            handle->GetDimension()
        );
        RenderCore::Get().UploadTexture(tex.get(), handle->GetPixels());

        UInt32 descriptor_index = tex->GetSRVHandle().index;
        texture.tex = std::move(tex);

        return descriptor_index;
    }

}