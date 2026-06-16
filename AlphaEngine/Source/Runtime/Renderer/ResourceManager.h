#pragma once
#include "Core/Subsystem.h"
#include "Core/Common.h"

namespace Alpha
{
    class ModelHandle;
    class MaterialHandle;
    class TextureHandle;
    class Buffer;
}

namespace Alpha::Renderer
{
    class StaticMesh;
    class SkeletalMesh;
    class Material;
    class Texture2D;


    class ResourceManager : public Subsystem<ResourceManager>
    {
    private:
        TMap<UInt64, TShared<SkeletalMesh>> m_mesh_cache{};

        TMap<UInt64, TShared<Material>> m_material_cache{};

        TMap<UInt64, TShared<Texture2D>> m_texture_cache{};

        TUnique<Buffer> m_base_position_pool{ nullptr };
        TUnique<Buffer> m_base_normal_pool{ nullptr };

        TUnique<Buffer> m_texcoord_pool{ nullptr };

        TUnique<Buffer> m_bone_indices_pool{ nullptr };
        TUnique<Buffer> m_bone_weights_pool{ nullptr };

        TUnique<Buffer> m_bone_matrices_pool{ nullptr };

        TUnique<Buffer> m_deformed_position_pool{ nullptr };
        TUnique<Buffer> m_deformed_normal_pool{ nullptr };

        TUnique<Buffer> m_index_pool{ nullptr };


    public:
        ~ResourceManager();
        struct Args
        {
            // 頂点プール（base・deformedは同サイズが基本
            // デフォルト値入れてるけど必要になったら外部で調整して
            UInt64 base_position_pool_size{ 48_MB };        // float3 × 4M頂点
            UInt64 base_normal_pool_size{ 48_MB };          // float3 × 4M頂点
            
            UInt64 texcoord_pool_size{ 32_MB };             // float2 × 4M頂点

            UInt64 bone_indices_pool_size{ 32_MB };         // uint16×4 × 4M頂点
            UInt64 bone_weights_pool_size{ 64_MB };         // float4 × 4M頂点

            UInt64 bone_matrices_pool_size{ 2_MB };         // float4x3(48B) × 256体 × 128bone

            UInt64 deformed_position_pool_size{ 48_MB };    // UAV, BLASソース
            UInt64 deformed_normal_pool_size{ 48_MB };      // UAV, BLASソース

			UInt64 index_pool_size{ 48_MB };                // uint32 × 12M

        };
        bool Initialize(const Args& args);
        void Finalize();

        TShared<SkeletalMesh> GetSkeletalMesh(TShared<ModelHandle> handle);
        TShared<Material> GetMaterial(TShared<MaterialHandle> handle);
        TShared<Texture2D> GetTexture(TShared<TextureHandle> handle);

    };
}