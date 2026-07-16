#include "PMXLoader.h"

#include "Asset/ModelHandle.h"
#include "Asset/MaterialHandle.h"
#include "Asset/MaterialTableHandle.h"
#include "Asset/TextureHandle.h"
#include "Asset/AssetManager.h"
#include "Animation/AnimationResource.h"
#include "PMXParser.h"

namespace Alpha
{
    TShared<AssetHandle> PMXLoader::AsyncLoad(StringView file_path)
    {
        PMXParser parser(file_path);
        auto handle = std::make_shared<ModelHandle>();

        parser.Parse();

        handle->SetName(parser.GetName());

        const TArray<PMXVertex>& pmx_vertices = parser.GetPMXVertices();
        
        TArray<Vector3> positions{};
        TArray<Vector3> normals{};
        TArray<Vector2> texcoords{};
        TArray<UVector4> bone_indices{};
        TArray<Vector4> bone_weights{};

        size_t vertex_count = pmx_vertices.size();
        positions.resize(vertex_count);
        normals.resize(vertex_count);
        texcoords.resize(vertex_count);

        if (parser.IsSkeletal()) 
        {
            bone_indices.resize(vertex_count);
            bone_weights.resize(vertex_count);
        }

        for (size_t i = 0; i < vertex_count; ++i)
        {
            const auto& v = pmx_vertices[i];
            positions[i] = v.position;
            normals[i] = v.normal;
            texcoords[i] = v.texcoord;

            if (parser.IsSkeletal()) 
            {
                bone_indices[i] = {v.boneIndex[0],  v.boneIndex[1],  v.boneIndex[2],  v.boneIndex[3]};
                bone_weights[i] = { v.boneWeight[0], v.boneWeight[1], v.boneWeight[2], v.boneWeight[3] };
            }
        }


        handle->SetBaseVeretices(std::move(positions), std::move(normals), std::move(texcoords));

        handle->SetIndices(std::move(parser.CreateIndices()));

        if (parser.IsSkeletal())
        {
            handle->SetSkeletalVertices(std::move(bone_indices), std::move(bone_weights), std::move(parser.CreateSkeleton()));
            
        }

        TShared<MaterialTableHandle> table = parser.CreateMaterials();
        TArray<SubMeshHandle> sub_meshes = parser.CreateSubMeshes();

        for (auto& material : table->GetMaterialHandles())
        {
            for (auto& texture : material->GetTextures())
            {
                auto tex_handle = AssetManager::Get().Load<TextureHandle>(texture.path);
                tex_handle->SetName(texture.path);
                texture.handle = tex_handle;
            }
        }

        handle->SetMaterialTable(std::move(table));
        handle->SetSubMeshes(std::move(sub_meshes));

        handle->BuildVertexAttributeBuffer();


        return handle;
    }

}