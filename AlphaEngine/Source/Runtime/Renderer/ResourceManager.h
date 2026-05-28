#pragma once
#include "Core/Subsystem.h"
#include "Core/Types.h"
#include "Asset/HandleHash.h"

namespace Alpha
{
    class ModelHandle;
    class MaterialHandle;
    class TextureHandle;
    class ByteAddressBuffer;
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
        TMap<uint64_t, TShared<SkeletalMesh>> m_mesh_cache{};

        TMap<uint64_t, TShared<Material>> m_material_cache{};

        TMap<uint64_t, TShared<Texture2D>> m_texture_cache{};

        TUnique<ByteAddressBuffer> m_base_position_pool{ nullptr };
        TUnique<ByteAddressBuffer> m_base_normal_pool{ nullptr };
        TUnique<ByteAddressBuffer> m_bone_indices_pool{ nullptr };
        TUnique<ByteAddressBuffer> m_bone_weights_pool{ nullptr };
        TUnique<ByteAddressBuffer> m_bone_matrices_pool{ nullptr };
        TUnique<ByteAddressBuffer> m_deformed_position_pool{ nullptr };
        TUnique<ByteAddressBuffer> m_deformed_normal_pool{ nullptr };

    public:
        ~ResourceManager();
        struct Args
        {

        };
        bool Initialize(const Args& args);
        void Finalize();

        TShared<SkeletalMesh> GetSkeletalMesh(TShared<ModelHandle> handle);
        TShared<Material> GetMaterial(TShared<MaterialHandle> handle);
        TShared<Texture2D> GetTexture(TShared<TextureHandle> handle);

    };
}