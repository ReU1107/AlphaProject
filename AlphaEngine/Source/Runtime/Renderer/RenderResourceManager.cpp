#include "RenderResourceManager.h"
#include "Asset/AssetManager.h"

#include "Asset/ModelHandle.h"
#include "Asset/MaterialHandle.h"
#include "Asset/MaterialTableHandle.h"
#include "Asset/TextureHandle.h"

#include "RenderCore/Module.h"

namespace Alpha::Renderer
{
	RenderResourceManager::~RenderResourceManager() = default;

	bool RenderResourceManager::Initialize(const Args& args)
	{
        m_vertex_position_allocator = CreatePersistentAllocator(args.render_core, sizeof(Vector3), 12_MB, EBufferPresetType::ROStructured, L"VertexPosition");
		m_vertex_attribute_allocator= CreatePersistentAllocator(args.render_core, sizeof(UInt32), 12_MB, EBufferPresetType::ROByteAddress, L"VertexAttribute");
		
        m_vertex_indices_allocator	= CreatePersistentAllocator(args.render_core, sizeof(UInt32), 12_MB, EBufferPresetType::ROStructured, L"VertexIndices");
		m_triangle_indices_allocator= CreatePersistentAllocator(args.render_core, sizeof(UInt32), 12_MB, EBufferPresetType::ROStructured, L"TriangleIndices");
		m_meshlet_allocator			= CreatePersistentAllocator(args.render_core, sizeof(Meshlet), 12_MB, EBufferPresetType::ROStructured, L"Meshlet");
		m_index_allocator			= CreatePersistentAllocator(args.render_core, sizeof(UInt32), 8_MB, EBufferPresetType::ROStructured, L"Index");
		
        m_mesh_allocator			= CreatePersistentAllocator(args.render_core, sizeof(MeshData), 4_MB, EBufferPresetType::ROStructured, L"Mesh");
        m_submesh_allocator			= CreatePersistentAllocator(args.render_core, sizeof(Submesh), 4_MB, EBufferPresetType::ROStructured, L"Submesh");
		
        m_material_allocator		= CreatePersistentAllocator(args.render_core, sizeof(MaterialData), 4_MB, EBufferPresetType::ROStructured, L"Material");

        m_material_table_allocator  = CreatePersistentAllocator(args.render_core, sizeof(UInt32), 4_MB, EBufferPresetType::ROStructured, L"MaterialTable");
        
        
        m_source_as_allocator       = CreatePersistentAllocator(args.render_core, 256, 32_MB, EBufferPresetType::RTAccelStructure, L"SourceAS");

        m_gpu_data_transfer = std::make_unique<GPUDataTransfer>();
        {
            GPUDataTransfer::Args desc = {
                .render_core = args.render_core,
                .staging_buffer_size = 64_MB,
            };
            m_gpu_data_transfer->Initialize(desc);
        }

        m_as_builder = std::make_unique<ASBuilder>();
        {
            ASBuilder::Args desc = {
                .render_core = args.render_core,
                .staging_buffer_size = 32_MB,
            };
            m_as_builder->Initialize(desc);
        }

        m_mesh_palette.Initialize(100);
        m_material_palette.Initialize(100);
        m_material_table_palette.Initialize(100);
        m_texture_palette.Initialize(100);


        // テクスチャ
        auto white_tex = args.asset_manager->GetWhiteTexture();
        m_default_white_texture_index = RegisterTexture(white_tex);
        ALPHA_CHECK_FATAL(m_default_white_texture_index != 0xFFFFFFFF, L"白テクスチャの登録に失敗しました。");

        auto normal_tex = args.asset_manager->GetNormalTexture();
        m_default_normal_texture_index = RegisterTexture(normal_tex);
        ALPHA_CHECK_FATAL(m_default_normal_texture_index != 0xFFFFFFFF, L"法線テクスチャの登録に失敗しました。");

        auto black_tex = args.asset_manager->GetBlackTexture();
        m_default_black_texture_index = RegisterTexture(black_tex);
        ALPHA_CHECK_FATAL(m_default_black_texture_index != 0xFFFFFFFF, L"黒テクスチャの登録に失敗しました。");

        // メッシュ
        auto cube = args.asset_manager->GetCube();
        m_default_mesh_index = RegisterMesh(cube);
        ALPHA_CHECK_FATAL(m_default_mesh_index != 0xFFFFFFFF, L"デフォルトキューブの登録に失敗しました。");

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

        // indexデータ
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
        // サブメッシュ
        {
            for (auto& submesh_handle : handle->GetSubMeshHandles())
            {
                Submesh sub_mesh = {
                    .index_element_offset = submesh_handle.index_element_offset,
                    .index_count = submesh_handle.index_count,
                };

                mesh.submeshes.push_back(sub_mesh);
            }

            UInt32 num = mesh.submeshes.size();
            Allocation alloc{};
            if (!m_submesh_allocator->AllocateByNum(num, &alloc))
            {
                ALPHA_LOG_WARN(L"ModelHandleのリソース化に失敗しました");
                // 
            }
            RenderCore::Get().UploadBuffer(m_submesh_allocator->GetBuffer(), alloc.offset, mesh.submeshes.data(), alloc.size);

            mesh.submesh_alloc = alloc;
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

            mesh.data.index_element_offset = mesh.index_alloc.offset / sizeof(UInt32);
            mesh.data.submesh_element_offset = mesh.submesh_alloc.offset / sizeof(Submesh);

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
        {
            mesh.default_table_index = RegisterMaterialTable(handle->GetMaterialTableHandle());
            mesh.data.material_table_element_offset = m_material_table_palette[mesh.default_table_index].alloc.offset / sizeof(UInt32);
        }

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


            for (auto& sub : mesh.submeshes)
            {
                request.geometries.push_back(
                    {
                        .index_byte_offset = sub.index_element_offset * sizeof(UInt32),
                        .index_count = sub.index_count,
                        .is_opaque = true,
                    }
                );
            }

            ASBuildSize build_size = RenderCore::Get().PreBuildBLAS(request);


            Allocation alloc{};
            m_source_as_allocator->AllocateBySize(build_size.result, &alloc);

            mesh.as.size = build_size.result;
            mesh.as.scratch_size = build_size.scratch;
            mesh.as.source_alloc = alloc;

            request.result_offset = mesh.as.source_alloc.offset;
            request.scratch_size = mesh.as.scratch_size;

            RenderCore::Get().GetASBuilder()->AddBuildRequest(request);

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
            UInt32 num = table.material_num;
            Allocation alloc{};
            if (!m_material_table_allocator->AllocateByNum(num, &alloc))
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

        auto white_tex = m_default_white_texture_index;
        auto black_tex = m_default_black_texture_index;
        auto normal_tex = m_default_normal_texture_index;

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

    TArray<Renderer::MaterialData> RenderResourceManager::GetDefaultMaterials(UInt64 mesh_index)
    {
        auto& mesh = GetMeshByIndex(mesh_index);
        
        auto& material_table = m_material_table_palette.GetAt(mesh.default_table_index);

        TArray<MaterialData> result(material_table.material_num);
        for (UInt32 i = 0; i < material_table.material_num; ++i)
        {
            result[i] = m_material_palette.GetAt(material_table.material_indices[i]).data;
        }

        return result;
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

    UInt32 RenderResourceManager::GetGPUTextureOffset(UInt64 index)
    {
        auto& texture = m_texture_palette.GetAt(index);
        return texture.tex->GetSRVHandle().index;
    }

    const TextureResource& RenderResourceManager::GetTextureByIndex(UInt64 index) const
    {
        return m_texture_palette.GetAt(index);
    }

    RenderResourceManager::Buffers RenderResourceManager::GetBuffers() const
    {
        return Buffers{
            .position = m_vertex_position_allocator->GetBuffer(),
            .attribute = m_vertex_attribute_allocator->GetBuffer(),
            .index = m_index_allocator->GetBuffer(),
            .source_as = m_source_as_allocator->GetBuffer(),
        };
    }

    StaticBufferIndices RenderResourceManager::GetBufferIndices() const
    {
        return StaticBufferIndices{
            .position = m_vertex_position_allocator->GetSRVIndex(),
            .attribute = m_vertex_attribute_allocator->GetSRVIndex(),
            .index = m_index_allocator->GetSRVIndex(),
            .vertex_indices = m_vertex_indices_allocator->GetSRVIndex(),
            .triangle_indices = m_triangle_indices_allocator->GetSRVIndex(),
            .meshlet = m_meshlet_allocator->GetSRVIndex(),
            .mesh = m_mesh_allocator->GetSRVIndex(),
            .submesh = m_submesh_allocator->GetSRVIndex(),
            .material = m_material_allocator->GetSRVIndex(),
            .material_table = m_material_table_allocator->GetSRVIndex()
        };
    }
}