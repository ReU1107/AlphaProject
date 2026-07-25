#include "RenderResourceManager.h"
#include "Asset/ModelHandle.h"
#include "Asset/MaterialHandle.h"
#include "Asset/MaterialTableHandle.h"
#include "Asset/TextureHandle.h"
#include "Asset/AssetManager.h"

#include "RenderCore/Module.h"

namespace Alpha
{
    using namespace Renderer;
	RenderResourceManager::~RenderResourceManager() = default;

	bool RenderResourceManager::Initialize(const Args& args)
	{

        m_vertex_position_allocator = CreatePersistentAllocator(args.render_core, sizeof(Vector3), 12_MB, EBufferPresetType::ROStructured, L"VertexPositionBuffer");
		m_vertex_attribute_allocator= CreatePersistentAllocator(args.render_core, sizeof(UInt32), 12_MB, EBufferPresetType::ROByteAddress, L"VertexAttributeBuffer");
		
        m_vertex_indices_allocator	= CreatePersistentAllocator(args.render_core, sizeof(UInt32), 12_MB, EBufferPresetType::ROStructured, L"VertexIndicesBuffer");
		m_triangle_indices_allocator= CreatePersistentAllocator(args.render_core, sizeof(UInt32), 12_MB, EBufferPresetType::ROStructured, L"TriangleIndicesBuffer");
		m_meshlet_allocator			= CreatePersistentAllocator(args.render_core, sizeof(Meshlet), 12_MB, EBufferPresetType::ROStructured, L"MeshletBuffer");
		m_index_allocator			= CreatePersistentAllocator(args.render_core, sizeof(UInt32), 8_MB, EBufferPresetType::ROStructured, L"IndexBuffer");
		
        m_mesh_allocator			= CreatePersistentAllocator(args.render_core, sizeof(MeshData), 4_MB, EBufferPresetType::ROStructured, L"ModelBuffer");
		
        m_material_allocator		= CreatePersistentAllocator(args.render_core, sizeof(MaterialData), 4_MB, EBufferPresetType::ROStructured, L"MaterialBuffer");

        m_material_table_allocator  = CreatePersistentAllocator(args.render_core, sizeof(UInt32), 4_MB, EBufferPresetType::ROByteAddress, L"MaterialTableBuffer");
        
        
        m_source_as_allocator       = CreatePersistentAllocator(args.render_core, 256, 32_MB, EBufferPresetType::RTAccelStructure, L"SourceASBuffer");




        m_mesh_palette.Initialize(100);
        m_material_palette.Initialize(100);
        m_material_table_palette.Initialize(100);
        m_texture_palette.Initialize(100);

		return true;
	}

	void RenderResourceManager::Finalize()
	{

	}

	UInt32 RenderResourceManager::RegisterMesh(TShared<ModelHandle> handle)
	{
        UInt64 handle_id = handle->GetID();
        UInt32 palette_index = 0;

        if (m_mesh_palette.TryGetIndex(handle_id, &palette_index))
        {
            return palette_index;
        }

        Mesh& mesh = m_mesh_palette.Allocate(handle_id, palette_index);

        // 座標データ
        {
			UInt32 num = handle->GetPositions().size();
			Allocation alloc{};
			if (!m_vertex_position_allocator->AllocateByNum(num, &alloc))
			{
				ALPHA_LOG_WARN(L"ModelHandleのリソース化に失敗しました");
				// 
			}
			
            RenderCore::Get().UploadBuffer(m_vertex_position_allocator->GetBuffer(), alloc.offset, handle->GetPositions().data(), alloc.size);

            mesh.vertex_position_alloc = alloc;
            mesh.vertex_count = num;
		}

        // 頂点属性データ
		{
			UInt32 size = handle->GetPackData().size();
			Allocation alloc{};
			if (!m_vertex_attribute_allocator->AllocateBySize(size, &alloc))
			{
				ALPHA_LOG_WARN(L"ModelHandleのリソース化に失敗しました");
				// 
			}
			RenderCore::Get().UploadBuffer(m_vertex_attribute_allocator->GetBuffer(), alloc.offset, handle->GetPackData().data(), alloc.size);

            mesh.vertex_attribute_alloc = alloc;
		}

        // indexデータ(DXR用)
        {
            UInt32 size = handle->GetIndices().size();
            Allocation alloc{};
            if (!m_index_allocator->AllocateByNum(size, &alloc))
            {
                ALPHA_LOG_WARN(L"ModelHandleのリソース化に失敗しました");
                // 
            }
            RenderCore::Get().UploadBuffer(m_index_allocator->GetBuffer(), alloc.offset, handle->GetIndices().data(), alloc.size);

            mesh.index_alloc = alloc;
        }

        // メッシュレットの頂点index
        {
            /*
            * m_vertex_indicesの中身に対してglobal_vertex_offsetを加算した値が全体の頂点バッファのオフセットになる
            */
            UInt32 num = handle->GetVertexIndices().size();
            Allocation alloc = {};
            m_vertex_indices_allocator->AllocateByNum(num, &alloc);
            RenderCore::Get().UploadBuffer(m_vertex_indices_allocator->GetBuffer(), alloc.offset, handle->GetVertexIndices().data(), alloc.size);
            
            /*
            * meshletのoffsetは頂点バッファのオフセットではなく、頂点インデックスバッファのオフセットを指す必要がある
            * そのためこのoffsetをmeshletのvertex_offsetに加算して設定する必要がある
            */
            mesh.vertex_inidices_alloc = alloc;

        }
        // メッシュレットの三角形index
        {
            UInt32 num = handle->GetTriangleIndices().size();
            Allocation alloc = {};
            m_triangle_indices_allocator->AllocateByNum(num, &alloc);
            RenderCore::Get().UploadBuffer(m_triangle_indices_allocator->GetBuffer(), alloc.offset, handle->GetTriangleIndices().data(), alloc.size);
            /*
            * meshletのtriangle_offsetは三角形インデックスバッファのオフセットを指す必要がある
            * そのためこのoffsetをmeshletのtriangle_offsetに加算して設定する必要がある
            */
            mesh.triangle_indices_alloc = alloc;
        }

        // メッシュレット
        {
            Allocation alloc = {};
            UInt32 num = handle->GetMeshlets().size();
            m_meshlet_allocator->AllocateByNum(num, &alloc);
            RenderCore::Get().UploadBuffer(m_meshlet_allocator->GetBuffer(), alloc.offset, handle->GetMeshlets().data(), alloc.size);

            mesh.meshlet_alloc = alloc;
        }

        // メッシュデータ
        {
            mesh.data.vertex_position_element_offset = mesh.vertex_position_alloc.offset / sizeof(Vector3);
            mesh.data.vertex_attribute_byte_offset = mesh.vertex_attribute_alloc.offset;  // ByteAddressBufferだからそのまま

            mesh.data.meshlet_element_offset = mesh.meshlet_alloc.offset / sizeof(Meshlet);
            mesh.data.meshlet_count = handle->GetMeshlets().size();
            mesh.data.vertex_element_offset = mesh.vertex_inidices_alloc.offset / sizeof(UInt32);
            mesh.data.triangle_element_offset = mesh.triangle_indices_alloc.offset / sizeof(UInt32);

            mesh.data.vertex_stride = handle->GetVertexAttributeStride();
            mesh.data.attribute_flags = handle->GetVertexAttributeFlags().GetValue();

            Allocation alloc{};
            m_mesh_allocator->AllocateByNum(1, &alloc);

            RenderCore::Get().UploadBuffer(m_mesh_allocator->GetBuffer(), alloc.offset, &mesh.data, alloc.size);

            mesh.data_alloc = alloc;
        }
        // マテリアルテーブルの登録
        mesh.default_table_index = RegisterMaterialTable(handle->GetMaterialTableHandle());

        // レイトレ用のAS作成
        {
            BLASRequest request = {
                .vertex_buffer = m_vertex_position_allocator->GetBuffer(),
                .vertex_offset = mesh.vertex_position_alloc.offset,
                .index_buffer = m_index_allocator->GetBuffer(),
                .index_offset = mesh.index_alloc.offset,
                .result_buffer = m_source_as_allocator->GetBuffer(),
                .vertex_count = mesh.vertex_count,
            };

            for (auto& sub_mesh_handle : handle->GetSubMeshHandles())
            {
                BLASRequest::Sub sub = {
                    .index_byte_offset = sub_mesh_handle.index_element_offset * sizeof(UInt32),
                    .index_count = sub_mesh_handle.index_count,
                };
               
                request.geometries.push_back(sub);
            }

            ASBuildSize build_size = RenderCore::Get().PreBuildBLAS(request);


            Allocation alloc{};
            m_source_as_allocator->AllocateBySize(build_size.result, &alloc);

            mesh.as.size = build_size.result;
            mesh.as.scratch_size = build_size.scratch;
            mesh.as.source_alloc = alloc;

            request.result_offset = mesh.as.source_alloc.offset;
            request.scratch_size = mesh.as.scratch_size,

            RenderCore::Get().BuildBLAS(request);


        }
        return palette_index;
	}

    UInt32 RenderResourceManager::RegisterMaterialTable(TShared<MaterialTableHandle> handle)
    {
        UInt64 handle_id = handle->GetID();
        UInt32 palette_index = 0;

        if (m_material_table_palette.TryGetIndex(handle_id, &palette_index))
        {
            return palette_index;
        }

        MaterialTable& table = m_material_table_palette.Allocate(handle_id, palette_index);

        // マテリアルの登録とこのモデルのデフォルトのマテリアルテーブルの作成
        for (auto& material_handle : handle->GetMaterialHandles())
        {
            UInt32 mat_index = RegisterMaterial(material_handle);


            table.material_indices[table.material_num++] = m_material_palette[mat_index].alloc.offset / sizeof(MaterialData);
        }

        {
            UInt32 size = sizeof(UInt32) * table.material_num;
            Allocation alloc{};
            if (!m_material_table_allocator->AllocateBySize(size, &alloc))
            {
                ALPHA_LOG_WARN(L"ModelHandleのリソース化に失敗しました");
                // 
            }
            RenderCore::Get().UploadBuffer(m_material_table_allocator->GetBuffer(), alloc.offset, table.material_indices, alloc.size);

            table.alloc = alloc;
        }

        return palette_index;
    }

    UInt32 RenderResourceManager::RegisterMaterial(TShared<MaterialHandle> handle)
    {
        UInt64 handle_id = handle->GetID();
        UInt32 palette_index = 0;

        if (m_material_palette.TryGetIndex(handle_id, &palette_index))
        {
            return palette_index;
        }

        Material& material = m_material_palette.Allocate(handle_id, palette_index);

        TMap<ETextureAttribute, UInt32> local_tex_cache;

        for (auto& texture : handle->GetTextures())
        {
            ETextureAttribute attr = texture.attribute;
            if (attr == ETextureAttribute::None) 
                continue;

            local_tex_cache[attr] = RegisterTexture(texture.handle);
        }

        auto white_tex = RegisterTexture(AssetManager::Get().GetWhiteTexture());
        auto black_tex = RegisterTexture(AssetManager::Get().GetBlackTexture());
        auto normal_tex = RegisterTexture(AssetManager::Get().GetNormalTexture());

        auto GetTexture = [&](ETextureAttribute attr, UInt32 fallback_index)
        {
            auto found = local_tex_cache.find(attr);
            
            UInt32 index = fallback_index;
            if (found != local_tex_cache.cend())
                index = found->second;

            return m_texture_palette[index].tex.get()->GetSRVHandle().index;
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

        RenderCore::Get().UploadBuffer(m_material_allocator->GetBuffer(), alloc.offset, &material.data, alloc.size);

        material.alloc = alloc;
        return palette_index;
    }

    UInt32 RenderResourceManager::RegisterTexture(TShared<TextureHandle> handle)
    {
        UInt64 handle_id = handle->GetID();
        UInt32 palette_index = 0;

        if (m_texture_palette.TryGetIndex(handle_id, &palette_index))
        {
            return palette_index;
        }

        TextureResource& texture = m_texture_palette.Allocate(handle_id, palette_index);

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
        texture.tex = std::move(tex);

        return palette_index;
    }

    const Renderer::Mesh& RenderResourceManager::GetMeshByHandle(TShared<ModelHandle> handle)
    {
        UInt64 handle_id = handle->GetID();

        return GetMeshByHandle(handle_id);
    }

    const Renderer::Mesh& RenderResourceManager::GetMeshByHandle(UInt64 handle_id)
    {
        UInt32 palette_index = 0;

        if (!m_mesh_palette.TryGetIndex(handle_id, &palette_index))
        {
            ALPHA_LOG_WARN(L"");
        }


        return GetMeshByIndex(palette_index);
    }

    const Mesh& RenderResourceManager::GetMeshByIndex(UInt64 index)
    {
        return m_mesh_palette.GetAt(index);
    }

    UInt32 RenderResourceManager::GetMaterialTableIndexByMesh(UInt64 index)
    {
        auto& mesh = GetMeshByIndex(index);

        return mesh.default_table_index;
    }

    const Renderer::Material& RenderResourceManager::GetMaterialByIndex(UInt64 index)
    {
        return m_material_palette.GetAt(index);
    }

    const Renderer::MaterialTable& RenderResourceManager::GetMaterialTableByIndex(UInt64 index)
    {
        return m_material_table_palette.GetAt(index);
    }

    const Renderer::MaterialTable& RenderResourceManager::GetMaterialTableByMeshIndex(UInt64 index)
    {
        auto& mesh = GetMeshByIndex(index);

        return GetMaterialTableByIndex(mesh.default_table_index);
    }

    UInt32 RenderResourceManager::GetGPUMeshOffset(UInt64 index)
    {
        auto& mesh = GetMeshByIndex(index);

        return mesh.data_alloc.offset / sizeof(MeshData);
    }

    UInt32 RenderResourceManager::GetGPUMaterialTableOffset(UInt64 index)
    {
        auto& material_table = GetMaterialTableByIndex(index);
        return material_table.alloc.offset;
    }

    RenderResourceManager::BufferIndices RenderResourceManager::GetBufferIndices() const
    {
        return BufferIndices{
            .position = m_vertex_position_allocator->GetSRVIndex(),
            .attribute = m_vertex_attribute_allocator->GetSRVIndex(),
            .vertex_indices = m_vertex_indices_allocator->GetSRVIndex(),
            .triangle_indices = m_triangle_indices_allocator->GetSRVIndex(),
            .meshlet = m_meshlet_allocator->GetSRVIndex(),
            .mesh = m_mesh_allocator->GetSRVIndex(),
            .material = m_material_allocator->GetSRVIndex(),
            .material_table = m_material_table_allocator->GetSRVIndex()
        };
    }
}