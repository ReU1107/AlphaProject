#pragma once
#include "Core/Common.h"
#include "Core/Subsystem.h"
#include "Core/Container/SlotPalette.h"
#include "Definitions.h"

namespace Alpha
{
    class AssetManager;
    class ModelHandle;
    class MaterialHandle;
    class MaterialTableHandle;
    class TextureHandle;

    class RenderCore;
    class GPUDataTransfer;
    class ASBuilder;
    class PersistentBufferAllocator;
}

namespace Alpha::Renderer
{

    class RenderResourceManager : public Subsystem<RenderResourceManager>
    {
    private:
        TSlotPalette<Mesh> m_mesh_palette{};
        TSlotPalette<Material> m_material_palette{};
        TSlotPalette<MaterialTable> m_material_table_palette{};
        TSlotPalette<TextureResource> m_texture_palette{};

        TUnique<PersistentBufferAllocator> m_vertex_position_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_vertex_attribute_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_vertex_indices_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_triangle_indices_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_meshlet_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_index_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_submesh_allocator{ nullptr };

        TUnique<PersistentBufferAllocator> m_source_as_allocator{ nullptr };

        TUnique<PersistentBufferAllocator> m_mesh_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_material_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_material_table_allocator{ nullptr };

        // RenderCoreから引っ越しするかも?
        TUnique<GPUDataTransfer> m_gpu_data_transfer{};
        TUnique<ASBuilder> m_as_builder{};


        UInt32 m_default_mesh_index{ 0 };
        UInt32 m_default_white_texture_index{ 0 };
        UInt32 m_default_normal_texture_index{ 0 };
        UInt32 m_default_black_texture_index{ 0 };

    public:
        ~RenderResourceManager();
        struct Args
        {
            RenderCore* render_core{ nullptr };
            AssetManager* asset_manager{ nullptr };
        };
        bool Initialize(const Args& args);
        void Finalize();

        UInt32 RegisterMesh(TShared<ModelHandle> handle);
        UInt32 RegisterMaterialTable(TShared<MaterialTableHandle> handle);
        UInt32 RegisterMaterial(TShared<MaterialHandle> handle);
        UInt32 RegisterTexture(TShared<TextureHandle> handle);

        const Mesh& GetMeshByHandle(TShared<ModelHandle> handle);
        const Mesh& GetMeshByHandle(UInt64 id);
        const Mesh& GetMeshByIndex(UInt64 index);
        TArray<MaterialData> GetDefaultMaterials(UInt64 mesh_index);

        const Material& GetMaterialByIndex(UInt64 index);
        const MaterialTable& GetMaterialTableByIndex(UInt64 index);
        const MaterialTable& GetMaterialTableByMeshIndex(UInt64 index);

        UInt32 GetGPUMeshOffset(UInt64 index);
        UInt32 GetGPUMaterialTableOffset(UInt64 index);

		UInt32 GetGPUTextureOffset(UInt64 index);

		const TextureResource& GetTextureByIndex(UInt64 index) const;

        struct Buffers
        {
            Buffer* position{ nullptr };
            Buffer* attribute{ nullptr };
            Buffer* index{ nullptr };
            Buffer* source_as{ nullptr };
        };

        Buffers GetBuffers() const;

        StaticBufferIndices GetBufferIndices() const;
    };
}