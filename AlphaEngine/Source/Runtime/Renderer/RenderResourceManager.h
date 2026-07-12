#pragma once
#include "Core/Subsystem.h"
#include "Core/Common.h"
#include "Core/Allocator/SlotAllocator.h"
#include "Definitions.h"

namespace Alpha
{
    class ModelHandle;
    class MaterialHandle;
    class TextureHandle;
    class PersistentBufferAllocator;

    // Tの実体オブジェクト配列をハッシュとindexで監理するクラス
    // 実体を先に作っておきそこから配布する感じの作りだから動的変化はしないつもりではいる
    template <typename T>
    class TSlotPalette
    {
    private:
        TMap<UInt64, UInt32> m_key_to_index_map{};
        TArray<T> m_palette{};
        SlotAllocator m_allocator{};

    public:

        void Initialize(UInt32 max_element_count)
        {
            m_palette.resize(max_element_count);

            m_allocator.Initialize(max_element_count);
        }

        bool Contains(UInt64 key)
        {
            return (m_key_to_index_map.contains(key));
        }

        T& Allocate(UInt64 key)
        {
            auto found = m_key_to_index_map.find(key);
            if (found != m_key_to_index_map.cend())
                return m_palette[found->second];

            UInt32 index = 0;
            if (!m_allocator.Allocate(&index))
            {
                ALPHA_LOG_WARN(L"想定した量を超えています初期化時の数を増やすなどし対処してください");
                // いちおう動くためにバッファの数を一旦増やす

            }

            m_key_to_index_map[key] = index;

            return m_palette[index];
        }

        void Free(UInt64 key)
        {
            // 登録されているか確認
            if (!m_key_to_index_map.contains(key))
            {
                ALPHA_LOG_WARN(L"登録されていないキーを解放しようとしました: %llu", key);
                return;
            }

            UInt32 index = m_key_to_index_map[key];

            m_allocator.Free(index);

            m_palette[index] = T{};

            m_key_to_index_map.erase(key);
        }

    };

    /*
    * GPUResourceManager
    */

    class RenderResourceManager : public Subsystem<RenderResourceManager>
    {
    private:
        TSlotPalette<Renderer::Mesh> m_mesh_palette{};
        TSlotPalette<Renderer::Material> m_material_palette{};
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

        };
        bool Initialize(const Args& args);
        void Finalize();

        UInt32 RegisterMesh(TShared<ModelHandle> handle);
        void RegisterMaterial(TShared<MaterialHandle> handle);

        UInt32 RegisterTexture(TShared<TextureHandle> handle);

    };
}