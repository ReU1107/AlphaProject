#include "PMXLoader.h"

#include "Asset/ModelHandle.h"
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

        if (parser.IsSkeletal())
        {
            

            handle->m_skeletal_vertices = parser.CreateSkeletalVertices();
            handle->m_indices = parser.CreateIndices();
            handle->m_materials = parser.CreateMaterials();
            handle->m_sub_meshes = parser.CreateSubMeshes();

            handle->m_skeleton = parser.CreateSkeleton();
            handle->m_mesh_type = Renderer::EMeshType::Skeletal;
        }
        else
        {
            handle->m_static_vertices = parser.CreateStaticVertices();
            handle->m_indices = parser.CreateIndices();
            handle->m_materials = parser.CreateMaterials();
            handle->m_sub_meshes = parser.CreateSubMeshes();
            handle->m_mesh_type = Renderer::EMeshType::Static;

        }

        return handle;
    }

}