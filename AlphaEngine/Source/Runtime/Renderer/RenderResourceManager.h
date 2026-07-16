#pragma once
#include "Core/Subsystem.h"
#include "Core/Common.h"
#include "Core/Container/SlotPalette.h"
#include "Definitions.h"

namespace Alpha
{
    class ModelHandle;
    class MaterialHandle;
    class MaterialTableHandle;
    class TextureHandle;

    class RenderCore;
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

        TUnique<PersistentBufferAllocator> m_mesh_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_material_allocator{ nullptr };
        TUnique<PersistentBufferAllocator> m_material_table_allocator{ nullptr };


    public:
        ~RenderResourceManager();
        struct Args
        {
            RenderCore* render_core{ nullptr };
        };
        bool Initialize(const Args& args);
        void Finalize();

        UInt32 RegisterMesh(TShared<ModelHandle> handle);
        UInt32 RegisterMaterialTable(TShared<MaterialTableHandle> handle);
        UInt32 RegisterMaterial(TShared<MaterialHandle> handle);
        UInt32 RegisterTexture(TShared<TextureHandle> handle);

        const Renderer::Mesh& GetMesh(UInt64 id);

        struct HeapIndices
        {
            UInt32 position;
            UInt32 attribute;
            UInt32 vertex_indices;
            UInt32 triangle_indices;
            UInt32 meshlet;
            UInt32 mesh;
            UInt32 material;
            UInt32 material_table;
        };

        HeapIndices GetBufferHeapIndices() const;

        UInt32 GetPositionBufferIndex() const;
        UInt32 GetAttributeBufferIndex() const;
        UInt32 GetVertexIndicesBufferIndex() const;
        UInt32 GetTriangleIndicesBufferIndex() const;
        UInt32 GetMeshletBufferIndex() const;
        UInt32 GetMeshBufferIndex() const;
        UInt32 GetMaterialBufferIndex() const;
        UInt32 GetMaterialTableBufferIndex() const;
    };
}