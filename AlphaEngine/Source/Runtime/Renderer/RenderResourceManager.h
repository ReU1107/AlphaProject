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
    class Buffer;
    class PersistentBufferAllocator;

    /*
    * GPUResourceManager
    */

    class RenderResourceManager : public Subsystem<RenderResourceManager>
    {
    private:
        TSlotPalette<Renderer::Mesh> m_mesh_palette{};
        TSlotPalette<Renderer::Material> m_material_palette{};
        TSlotPalette<Renderer::MaterialTable> m_material_table_palette{};
        TSlotPalette<Renderer::TextureResource> m_texture_palette{};

        TUnique<PersistentBufferAllocator> m_vertex_position_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_vertex_attribute_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_vertex_indices_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_triangle_indices_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_meshlet_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_index_allocator{ nullptr };

        TUnique<PersistentBufferAllocator> m_source_as_allocator{ nullptr };

        TUnique<PersistentBufferAllocator> m_mesh_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_material_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_material_table_allocator{ nullptr };


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

        const Renderer::Mesh& GetMeshByHandle(TShared<ModelHandle> handle);
        const Renderer::Mesh& GetMeshByHandle(UInt64 id);
        const Renderer::Mesh& GetMeshByIndex(UInt64 index);
        UInt32 GetMaterialTableIndexByMesh(UInt64 index);

        const Renderer::Material& GetMaterialByIndex(UInt64 index);
        const Renderer::MaterialTable& GetMaterialTableByIndex(UInt64 index);
        const Renderer::MaterialTable& GetMaterialTableByMeshIndex(UInt64 index);

        UInt32 GetGPUMeshOffset(UInt64 index);
        UInt32 GetGPUMaterialTableOffset(UInt64 index);

        struct Buffers
        {
            Buffer* position{ nullptr };
            Buffer* attribute{ nullptr };
            Buffer* index{ nullptr };
            Buffer* source_as{ nullptr };
        };

        Buffers GetBuffers() const;


        struct BufferIndices
        {
            UInt32 position{};
            UInt32 attribute{};
            UInt32 vertex_indices{};
            UInt32 triangle_indices{};
            UInt32 meshlet{};
            UInt32 mesh{};
            UInt32 material{};
            UInt32 material_table{};
        };

        BufferIndices GetBufferIndices() const;
    };
}