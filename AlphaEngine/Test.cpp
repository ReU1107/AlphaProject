
#include "BistroLocalLights.h"

#include <iostream>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#define NOMINMAX
#include <windows.h>

#include "Source/Core/Common.h"
#include "Source/Core/Window/WindowSystem.h"

#include "Source/Runtime/RenderCore/Module.h"
#include "Source/Runtime/Particle/Module.h"


#include "Source/Runtime/Asset/Module.h"
#include "Source/Runtime/Animation/Module.h"
#include "Source/Runtime/Renderer/Module.h"

#include "Source/Runtime/Scene/Module.h"


#include "Source/Core/Serialization/ByteWriter.h"

#include "ThirdParty/entt/entt.hpp"
#include "ThirdParty/imgui/imgui.h"

#include "Source/Runtime/System/RenderSystem.h"
#include "Source/Runtime/System/RenderSceneBuilder.h"
#include "Source/Runtime/System/CameraSystem.h"
#include "Source/Runtime/Renderer/CameraComponent.h"

#include "Source/Runtime/Spatial/TransformComponent.h"
#include "RHI/Definitions.h"

#include "Source/Core/Math/MathF.h"

#include "TestScene/Scene1.h"

#include "Source/Core/String/StringUtil.h"


#include <limits>
#include <stdexcept>

#include "ThirdParty/ufbx/ufbx.h"

#pragma comment(lib, "dxguid.lib")

using Microsoft::WRL::ComPtr;
using namespace Alpha;
using namespace Alpha::Renderer;
using namespace Alpha::Particle;
using namespace Alpha::Animation;
#include "BistroLocalLights_Alpha.inl"

struct FBXImportContext
{
    String asset_prefix;

    TMap<const ufbx_material*, TShared<MaterialHandle>> materials;
    TMap<const ufbx_texture*, TShared<TextureHandle>> textures;
};
ESamplerPresetType ConvertSampler(ufbx_wrap_mode wrap_u, ufbx_wrap_mode wrap_v)
{
    const bool u_wrap = wrap_u == UFBX_WRAP_REPEAT;
    const bool v_wrap = wrap_v == UFBX_WRAP_REPEAT;

    if (u_wrap && v_wrap)
        return ESamplerPresetType::LinearWrap;

    if (u_wrap)
        return ESamplerPresetType::LinearWrapClampClamp;

    if (v_wrap)
        return ESamplerPresetType::LinearClampWrapClamp;

    return ESamplerPresetType::LinearClamp;
}

TShared<TextureHandle> ConvertTexture(
    ufbx_texture* texture,
    FBXImportContext& context)
{
    if (!texture)
        return nullptr;

    auto found = context.textures.find(texture);
    if (found != context.textures.end())
        return found->second;

    // shader/layered textureの場合も、実ファイルへ辿れるなら最初のものを使う
    ufbx_texture* file_texture = texture;

    if (file_texture->type != UFBX_TEXTURE_FILE)
    {
        if (file_texture->file_textures.count == 0)
            return nullptr;

        file_texture =
            file_texture->file_textures.data[0];
    }

    if (file_texture->filename.length == 0)
        return nullptr;

    std::string path_utf8(
        file_texture->filename.data,
        file_texture->filename.length);

    std::filesystem::path fs_path(path_utf8);
    fs_path = fs_path.lexically_normal();
    fs_path.make_preferred();

    String path = fs_path.native();

    auto handle =
        AssetManager::Get().Load<TextureHandle>(path);
    handle->SetName(path);

    if (!handle)
        return nullptr;

    context.textures.emplace(texture, handle);

    return handle;
}

TShared<MaterialHandle> ConvertMaterial(
    ufbx_material* material,
    FBXImportContext& context)
{
    if (!material)
        return AssetManager::Get().GetDefaultMaterial();

    auto found = context.materials.find(material);
    if (found != context.materials.end())
        return found->second;

    auto handle =
        std::make_shared<MaterialHandle>();

    const String name =
        context.asset_prefix +
        L"/Material/" +
        std::to_wstring(material->typed_id);

    handle->SetName(name);

    const auto& pbr = material->pbr;

    // BaseColor
    if (pbr.base_color.texture &&
        pbr.base_color.texture_enabled)
    {
        auto texture =
            ConvertTexture(
            pbr.base_color.texture,
            context);
        auto tex = pbr.base_color.texture;
        if (texture)
        {
            auto format = texture->GetFormat();

            texture->SetFormat(ToSRGBFormat(format));

            ESamplerPresetType sampler = ConvertSampler(pbr.base_color.texture->wrap_u, pbr.base_color.texture->wrap_v);
            handle->AddTexture(
                ETextureAttribute::BaseColor,
                texture, sampler);

        }
    }

    // Normal
    if (pbr.normal_map.texture &&
        pbr.normal_map.texture_enabled)
    {
        auto texture =
            ConvertTexture(
            pbr.normal_map.texture,
            context);

        if (texture)
        {
            ESamplerPresetType sampler = ConvertSampler(pbr.normal_map.texture->wrap_u, pbr.normal_map.texture->wrap_v);
            handle->AddTexture(
                ETextureAttribute::Normal,
                texture, sampler);
        }
    }

    // Metalness
    if (pbr.metalness.texture &&
        pbr.metalness.texture_enabled)
    {

        auto texture =
            ConvertTexture(
            pbr.metalness.texture,
            context);

        if (texture)
        {
            ESamplerPresetType sampler = ConvertSampler(pbr.metalness.texture->wrap_u, pbr.metalness.texture->wrap_v);

            handle->AddTexture(
                ETextureAttribute::Metallic,
                texture, sampler);
        }
    }

    // Emissive
    if (pbr.emission_color.texture &&
        pbr.emission_color.texture_enabled)
    {
        auto texture =
            ConvertTexture(
            pbr.emission_color.texture,
            context);

        if (texture)
        {
            ESamplerPresetType sampler = ConvertSampler(pbr.emission_color.texture->wrap_u, pbr.emission_color.texture->wrap_v);
            handle->AddTexture(
                ETextureAttribute::Emissive,
                texture, sampler);
        }
    }

    // AO
    if (pbr.ambient_occlusion.texture &&
        pbr.ambient_occlusion.texture_enabled)
    {
        auto texture =
            ConvertTexture(
            pbr.ambient_occlusion.texture,
            context);

        if (texture)
        {
            ESamplerPresetType sampler = ConvertSampler(pbr.ambient_occlusion.texture->wrap_u, pbr.ambient_occlusion.texture->wrap_v);
            handle->AddTexture(
                ETextureAttribute::Occlusion,
                texture, sampler);
        }
    }

    AssetManager::Get().RegisterAsset(name, handle);

    context.materials.emplace(material, handle);

    return handle;
}


TShared<ModelHandle> ConvertModel(ufbx_mesh* mesh, FBXImportContext& context)
{
    if (!mesh || mesh->num_triangles == 0)
        return nullptr;

    if (!mesh->vertex_normal.exists)
        throw std::runtime_error("FBX mesh has no normals.");

    struct Vertex
    {
        float px;
        float py;
        float pz;

        float nx;
        float ny;
        float nz;

        float u;
        float v;
    };

    String mesh_name = std::filesystem::path(std::string(
        mesh->name.data,
        mesh->name.length)).c_str();


    String name =
        context.asset_prefix +
        L"/Mesh/" +
        mesh_name +
        L"_" +
        std::to_wstring(mesh->typed_id);


    auto model = std::make_shared<ModelHandle>();
    model->SetName(name);

    TArray<Vertex> vertices;
    TArray<UInt32> indices;
    TArray<SubMeshHandle> submeshes;

    TArray<uint32_t> triangle_indices(
        mesh->max_face_triangles * 3);

    auto material_table =
        std::make_shared<MaterialTableHandle>();
    material_table->SetName(model->GetName() + L"/MaterialTable");

    auto default_material =
        AssetManager::Get().GetDefaultMaterial();

    for (const ufbx_mesh_part& part : mesh->material_parts)
    {
        if (part.num_triangles == 0)
            continue;

        if (submeshes.size() >= 64)
            throw std::runtime_error("Too many material slots.");

        SubMeshHandle submesh{};

        submesh.material_index =
            static_cast<UInt32>(submeshes.size());

        // 現時点では1頂点 = 1 indexなので
        // vertices.size()がそのままindex offsetになる。
        submesh.index_element_offset =
            static_cast<UInt32>(vertices.size());

        for (uint32_t face_index : part.face_indices)
        {
            const ufbx_face face =
                mesh->faces[face_index];

            if (face.num_indices < 3)
                continue;

            const uint32_t triangle_count =
                ufbx_triangulate_face(
                triangle_indices.data(),
                triangle_indices.size(),
                mesh,
                face);

            for (size_t i = 0; i < size_t(triangle_count) * 3; ++i)
            {
                const uint32_t corner_index =
                    triangle_indices[i];

                const ufbx_vec3 position =
                    ufbx_get_vertex_vec3(
                    &mesh->vertex_position,
                    corner_index);

                const ufbx_vec3 normal =
                    ufbx_get_vertex_vec3(
                    &mesh->vertex_normal,
                    corner_index);

                ufbx_vec2 uv{};

                if (mesh->vertex_uv.exists)
                {
                    uv = ufbx_get_vertex_vec2(
                        &mesh->vertex_uv,
                        corner_index);
                }

                vertices.push_back(
                    {
                        static_cast<float>(position.x),
                        static_cast<float>(position.y),
                        static_cast<float>(position.z),

                        static_cast<float>(normal.x),
                        static_cast<float>(normal.y),
                        static_cast<float>(normal.z),

                        static_cast<float>(uv.x),
                        1.0f - static_cast<float>(uv.y),
                    });
            }
        }

        submesh.index_count =
            static_cast<UInt32>(vertices.size()) -
            submesh.index_element_offset;

        submeshes.push_back(submesh);


        TShared<MaterialHandle> material = default_material;

        if (part.index < mesh->materials.count)
        {
            if (ufbx_material* fbx_material = mesh->materials[part.index])
            {
                material =
                    ConvertMaterial(
                    fbx_material,
                    context);
            }
        }

        material_table->AddMaterial(material);
    }

    if (vertices.empty())
        return nullptr;

    indices.resize(vertices.size());

    ufbx_vertex_stream stream{};
    stream.data = vertices.data();
    stream.vertex_count = vertices.size();
    stream.vertex_size = sizeof(Vertex);

    ufbx_error error{};

    const size_t vertex_count =
        ufbx_generate_indices(
        &stream,
        1,
        indices.data(),
        indices.size(),
        nullptr,
        &error);

    if (error.type != UFBX_ERROR_NONE)
    {
        throw std::runtime_error(
            "ufbx_generate_indices() failed.");
    }

    vertices.resize(vertex_count);

    TArray<Vector3> positions;
    TArray<Vector3> normals;
    TArray<Vector2> texcoords;

    positions.reserve(vertex_count);
    normals.reserve(vertex_count);
    texcoords.reserve(vertex_count);

    for (const Vertex& vertex : vertices)
    {
        positions.emplace_back(
            vertex.px,
            vertex.py,
            vertex.pz);

        normals.emplace_back(
            vertex.nx,
            vertex.ny,
            vertex.nz);

        texcoords.emplace_back(
            vertex.u,
            vertex.v);
    }

    model->SetBaseVeretices(
        std::move(positions),
        std::move(normals),
        std::move(texcoords));

    model->SetIndices(std::move(indices));
    model->SetSubMeshes(std::move(submeshes));
    model->SetMaterialTable(material_table);

    model->BuildVertexAttributeBuffer();

    return model;
}

Matrix4x4 ConvertMatrix(const ufbx_matrix& m)
{
    // ufbx の行列を、Alpha の行ベクトル方式へ転置して格納。
    // 平行移動は第4行。
    return Matrix4x4(
        static_cast<float>(m.m00),
        static_cast<float>(m.m10),
        static_cast<float>(m.m20),
        0.0f,

        static_cast<float>(m.m01),
        static_cast<float>(m.m11),
        static_cast<float>(m.m21),
        0.0f,

        static_cast<float>(m.m02),
        static_cast<float>(m.m12),
        static_cast<float>(m.m22),
        0.0f,

        static_cast<float>(m.m03),
        static_cast<float>(m.m13),
        static_cast<float>(m.m23),
        1.0f);
}

void TestLoader(
    Scene& scene,
    const char* file_path,
    const String& asset_prefix)
{
    ufbx_load_opts opts{};

    opts.target_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
    opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Y;
    opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Z;

    opts.target_light_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
    opts.target_light_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Y;
    opts.target_light_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Z;
    opts.geometry_transform_handling = UFBX_GEOMETRY_TRANSFORM_HANDLING_HELPER_NODES;

    opts.inherit_mode_handling = UFBX_INHERIT_MODE_HANDLING_HELPER_NODES;

    opts.target_unit_meters = 1.0f;

    opts.generate_missing_normals = true;
    opts.load_external_files = true;
    opts.ignore_missing_external_files = true;

    ufbx_error error{};

    // 途中で例外が発生しても解放する。
    std::unique_ptr<ufbx_scene, decltype(&ufbx_free_scene)> fbx_scene(
        ufbx_load_file(file_path, &opts, &error),
        &ufbx_free_scene);

    if (!fbx_scene)
    {
        char message[2048]{};
        ufbx_format_error(message, sizeof(message), &error);
        throw std::runtime_error(message);
    }

    TMap<ufbx_mesh*, TShared<ModelHandle>> models;
    TMap<ufbx_mesh*, UInt32> mesh_indices;

    UInt32 model_number = 0;

    UInt64 total_positions = 0;
    UInt64 total_indices = 0;

    UInt64 total_fbx_vertices = 0;
    UInt64 total_generated_vertices = 0;

    FBXImportContext context{};
    context.asset_prefix = L"BistroExterior";

    for (ufbx_mesh* mesh : fbx_scene->meshes)
    {
        auto model = ConvertModel(mesh, context);
        if (!model)
            continue;


        // AssetManager は生成済みモデルを保持するだけ。
        AssetManager::Get().RegisterAsset(model->GetName(), model);

        const UInt32 mesh_index =
            RenderResourceManager::Get().RegisterMesh(model);

        models.emplace(mesh, model);
        mesh_indices.emplace(mesh, mesh_index);


        total_fbx_vertices += mesh->num_vertices;
        total_generated_vertices += model->GetPositions().size();
        total_indices += model->GetIndices().size();


    }
    UInt32 object_number = 0;

    TMap<ufbx_node*, SceneObject*> node_objects;

    for (ufbx_node* node : fbx_scene->nodes)
    {
        const String name =
            asset_prefix +
            L"/Object/" +
            std::to_wstring(object_number++);

        SceneObject& object = scene.CreateObject(name);
        node_objects[node] = &object;

        auto& transform = object.GetComponent<TransformComponent>();

        const ufbx_transform& src_transform = node->local_transform;

        Vector3 position(
            static_cast<float>(src_transform.translation.x),
            static_cast<float>(src_transform.translation.y),
            static_cast<float>(src_transform.translation.z));

        Quaternion rotation(
            static_cast<float>(src_transform.rotation.x),
            static_cast<float>(src_transform.rotation.y),
            static_cast<float>(src_transform.rotation.z),
            static_cast<float>(src_transform.rotation.w));

        Vector3 scale(
            static_cast<float>(src_transform.scale.x),
            static_cast<float>(src_transform.scale.y),
            static_cast<float>(src_transform.scale.z));

        transform.SetLocalPosition(position);
        transform.SetLocalRotation(rotation);
        transform.SetLocalScale(scale);

        if (node->mesh)
        {
            const auto found = mesh_indices.find(node->mesh);
            if (found != mesh_indices.end())
            {
                auto& mesh_component = object.AddComponent<MeshComponent>();
                mesh_component.mesh_index = found->second;
                mesh_component.enabled = true;
                mesh_component.visible = true;
            }
        }

        if (node->light)
        {
            ufbx_light* src_light = node->light;

            if (!src_light->cast_light)
                continue;

            UInt32 light_type = 0;

            switch (src_light->type)
            {
            case UFBX_LIGHT_POINT:
                light_type = 0;
                break;

            case UFBX_LIGHT_SPOT:
                light_type = 1;
                break;

            case UFBX_LIGHT_DIRECTIONAL:
                light_type = 2;
                object.AddComponent<MainLight>();

                break;

            default:
                continue;
            }

            String name = std::filesystem::path(std::string(node->name.data, node->name.length)).c_str();

            auto& light = object.AddComponent<LightComponent>();

            const ufbx_vec3 direction = ufbx_transform_direction(&node->node_to_world, src_light->local_direction);

            light.type = light_type;

            light.direction = VectorNormalize(Vector3(
                static_cast<float>(direction.x),
                static_cast<float>(direction.y),
                static_cast<float>(direction.z)));

            light.SetColor(float3(
                static_cast<float>(src_light->color.x),
                static_cast<float>(src_light->color.y),
                static_cast<float>(src_light->color.z)));

            light.SetIntensity(static_cast<float>(src_light->intensity));
            light.SetIntensity(1.0f);

            if (src_light->type == UFBX_LIGHT_SPOT)
            {
                light.SetSpotAngleDegree(
                    static_cast<float>(src_light->inner_angle),
                    static_cast<float>(src_light->outer_angle));
            }
        }

    }

    for (ufbx_node* node : fbx_scene->nodes)
    {
        auto child_it = node_objects.find(node);
        auto parent_it = node_objects.find(node->parent);

        if (child_it == node_objects.end() || parent_it == node_objects.end())
            continue;

        child_it->second->SetParent(parent_it->second);
    }

    BistroLighting::Options lighting_options{};
    lighting_options.intensityScale = 0.1f;
    lighting_options.stringIntensity = 40.f;
    lighting_options.stringRadius = 3.0f;

    // fbx_sceneが生きている間に実行。シーンの初回import時だけ呼ぶ。
    auto bistro_light_rig = AddBistroLocalLights(scene, fbx_scene.get(), asset_prefix, lighting_options);


}


AnimationBlueprint MakeLocomotionBlueprint(
    TShared<Clip> walk_clip,
    TShared<Clip> run_clip,
    TShared<Clip> dash_clip)
{
    AnimationBlueprint blueprint;
    auto& root = blueprint.GetRootMachine();
    root.name = L"Locomotion";
    root.play_rate = 1.0f;

    const auto walk_index = root.AddState(L"Walk");
    const auto run_index = root.AddState(L"Run");
    const auto dash_index = root.AddState(L"Dash");
    root.entry_state_index = walk_index;

    auto walk = root.GetState(walk_index);
    auto run = root.GetState(run_index);
    auto dash = root.GetState(dash_index);
    walk->play_rate = 1.0f;
    run->play_rate = 1.0f;
    dash->play_rate = 1.0f;

    walk->graph.AddLeafNode(std::move(walk_clip));
    run->graph.AddLeafNode(std::move(run_clip));
    dash->graph.AddLeafNode(std::move(dash_clip));

    auto add_speed_transition = [&](UInt32 source, UInt32 destination,
        ECompareType comparison, float threshold)
    {
        Condition condition{};
        condition.parameter_name = L"Speed";
        condition.threshold = threshold;
        condition.compare_type = comparison;

        TransitionDefinition transition;
        transition.dest_index = destination;
        transition.duration = 0.2f;
        transition.condition_type = EConditionType::All;
        transition.conditions.push_back(condition);
        root.GetChild(source)->transitions.push_back(std::move(transition));
    };

    add_speed_transition(walk_index, run_index, ECompareType::Greater, 2.0f);
    add_speed_transition(run_index, walk_index, ECompareType::Less, 2.0f);
    add_speed_transition(run_index, dash_index, ECompareType::Greater, 3.0f);
    add_speed_transition(dash_index, run_index, ECompareType::Less, 3.0f);
    return blueprint;
}

#include <thread>
#include <chrono>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
//int main()
{
    WindowSystem::Args win_args =
    {
        .title = L"AlphaEngine",
        .width = 1920,
        .height = 1080,
        .fullscreen = false
    };

    if (!WindowSystem::Startup(win_args))
        return -1;

    AssetManager::Args asset_args =
    {

    };
    AssetManager::Startup(asset_args);
    auto model1 = AssetManager::Get().Load<ModelHandle>(L"C:/Users/owner/Desktop/Asset/紲星☆あかり20180430/お宮式紲星☆あかりv1.00.pmx");
    auto model2 = AssetManager::Get().Load<ModelHandle>(L"C:/Users/owner/Desktop/Asset/パチュリーVer2.00/Model/パチュリーVer2.00_Type-C(フル_ポニテC_ミニスカ_ブーツC_パンプス)_サーモン.pmx");
    auto model3 = AssetManager::Get().GetSphere();
    auto model4 = AssetManager::Get().GetCube();

    auto walk = AssetManager::Get().Load<MotionHandle>(L"C:/Users/owner/Desktop/Asset/全親での移動モーション/歩きモーション/01_普通歩き【-33】.vmd");
    //auto walk = AssetManager::Get().Load(L"C:/Users/owner/Documents/Skinned/Project1/Assets/ハッピーシンセサイザ配布用モーション/GUMI.vmd");
    auto run = AssetManager::Get().Load<MotionHandle>(L"C:/Users/owner/Desktop/Asset/移動モーション_sm38179018/2_ランニング.vmd");
    auto dash = AssetManager::Get().Load<MotionHandle>(L"C:/Users/owner/Desktop/Asset/移動モーション_sm38179018/3_ダッシュ.vmd");

    auto skeleton = model1->GetSkeleton();


    walk->GetSkeletalClip()->Bind(skeleton);
    run->GetSkeletalClip()->Bind(skeleton);
    dash->GetSkeletalClip()->Bind(skeleton);

    auto blueprint = MakeLocomotionBlueprint(walk->GetSkeletalClip(), run->GetSkeletalClip(), dash->GetSkeletalClip());


    RenderCore::Args render_core_args = {};
    render_core_args.hWnd = (HWND)WindowSystem::Get().GetNativeHandle();
    render_core_args.transient_buffer_size = 1_MB;
    RenderCore::Startup(render_core_args);


    RenderResourceManager::Args resource_args = {
        .render_core = RenderCore::GetPtr(),
        .asset_manager = AssetManager::GetPtr(),
    };
    RenderResourceManager::Startup(resource_args);
   
    UInt32 akari_index = RenderResourceManager::Get().RegisterMesh(model1);
    UInt32 patchouli_index = RenderResourceManager::Get().RegisterMesh(model2);
    UInt32 sphere_index = RenderResourceManager::Get().RegisterMesh(model3);
    UInt32 cube_index = RenderResourceManager::Get().RegisterMesh(model4);

    RenderSystem::Args rs_args = {
        .render_core = RenderCore::GetPtr(),
    };
    RenderSystem render_system;
    render_system.Initialize(rs_args);


    Scene scene;
    scene.Initialize();

    auto& registry = scene.GetRegistry();

    SceneObject& object = scene.CreateObject(L"akari");

    auto& transform = object.GetComponent<TransformComponent>();

    transform.SetLocalPosition( Vector3(0.0f, 0.5f, 0.0f));

    auto& mesh = object.AddComponent<MeshComponent>();

    mesh.mesh_index = akari_index;
    mesh.visible = true;

    auto& anim = object.AddComponent<AnimationComponent>();
    anim.m_state_machine = blueprint.Instantiate();
    anim.m_pose = std::make_unique<SkeletonPose>();
    anim.m_pose->SetSkeleton(skeleton);
    anim.m_current_pose.bone_transforms.resize(512);

    anim.Play();

    {
        SceneObject& object =  scene.CreateObject(L"MainCamera");

        auto& transform = object.GetComponent<TransformComponent>();

        transform.SetLocalPosition( Vector3(-10.0f, 5.f, -10.0f));

        transform.LookAt(Vector3(0.0f, -0.023f, 1.0f));

        object.AddComponent<CameraComponent>();
    }

    TestLoader(
        scene,
        "C:/Users/owner/Desktop/Asset/Bistro_v5_2/BistroExterior.fbx",
        L"BistroExterior");

    using namespace std::chrono;

    // 60FPSの1フレームあたりの時間（約16.66ms）
    const duration<double, std::milli> target_frame_time(1000.0 / 60.0);

    CameraSystem cm_sys;
    AnimationSystem anim_sys;

    static float speed = 2.0f;

    int debug_value = 0;

    static const char* debug_view_names[] =
    {
        "None",
        "BaseColor",
        "WorldNormal",
        "UV",
        "MaterialID",
        "MaterialTableID",
        "NormalTexture",
        "BaseColorIndex",
        "UVRaw",
        "UVRange",
    };

    float intensity = 1.0f;

    while (WindowSystem::Get().ProcessMessages() && !WindowSystem::Get().ShouldClose())
    {
        auto frame_start = high_resolution_clock::now();

        scene.Update(1.0f / 60);

        cm_sys.Update(registry);

        anim_sys.Update(registry, 1.0f / 60);

        RenderCore::Get().BeginFrame();
        GUISystem::Get().BeginFrame();


        ImGui::DockSpaceOverViewport();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File")) { ImGui::EndMenu(); }
            if (ImGui::BeginMenu("Window")) { ImGui::EndMenu(); }
            ImGui::EndMainMenuBar();
        }

        ImGui::Begin("Hierarchy");
        ImGui::Text("Scene Root");
        if (ImGui::TreeNode("Main Camera")) { ImGui::TreePop(); }
        ImGui::End();

        ImGui::Begin("Inspector");

        auto camera_view = registry.view<CameraComponent, TransformComponent>();

        entt::entity camera_entity = camera_view.front();
        auto& camera = camera_view.get<CameraComponent>(camera_entity);
        auto& camera_t = camera_view.get<TransformComponent>(camera_entity);

        float* pos = camera_t.GetWorldMatrix()._4().data();

        ImGui::InputFloat3("Position", pos, "%.2f", ImGuiInputTextFlags_ReadOnly);

        ImGui::SeparatorText("Render Debug");

        ImGui::Combo(
            "Debug View",
            &debug_value,
            debug_view_names,
            IM_ARRAYSIZE(debug_view_names));

        auto main_light_entity = registry.view<MainLight, LightComponent>().front();
        auto& maint_light = registry.get<LightComponent>(main_light_entity);
        ImGui::SliderFloat("Intensity", &intensity, 0.0f, 120.0f);
        maint_light.SetIntensity(intensity);

        ImGui::End();

        ImGui::Begin("Application Info");

        const float fps = ImGui::GetIO().Framerate;
        const float frame_time = fps > 0.0f ? 1000.0f / fps : 0.0f;

        ImGui::Text("Application average: %.3f ms/frame (%.1f FPS)", frame_time, fps);

        ImGui::Separator();
        ImGui::Text("GPU Profile");

        auto* profiler = RenderCore::Get().GetGPUProfiler();
        auto& records = profiler->GetCollectedRecords();

        float gpu_frame_total_ms = 0.0f;
        for (auto& record : records)
        {
            gpu_frame_total_ms += record.elapsed_ms;
        }

        if (ImGui::BeginTable("GPU_Profile_Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            // 列の設定
            ImGui::TableSetupColumn("Pass", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Ratio", ImGuiTableColumnFlags_WidthFixed, 60.0f);
            ImGui::TableHeadersRow();

            for (auto& record : records)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", StringUtil::ConvertToUtf8(record.name).c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.3f ms", record.elapsed_ms);

                ImGui::TableSetColumnIndex(2);
                float percentage = (gpu_frame_total_ms > 0.0f) ? (record.elapsed_ms / gpu_frame_total_ms) * 100.0f : 0.0f;
                ImGui::Text("%.1f%%", percentage);
            }

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("GPU Frame:       %.3f ms", gpu_frame_total_ms);

        const float target_60hz_ms = 1000.0f / 60.0f;
        float headroom_ms = target_60hz_ms - gpu_frame_total_ms;

        if (headroom_ms < 0.0f) {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "GPU Headroom:    %.3f ms @60Hz (Over BUDGET!)", headroom_ms);
        }
        else {
            ImGui::Text("GPU Headroom:     %.3f ms @60Hz", headroom_ms);
        }

        ImGui::End();


        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");
        {
            ImVec2 viewport_size = ImGui::GetContentRegionAvail();

            Texture* lit_scene_tex = render_system.GetGraph()->FindTexture(L"LitSceneColor");

            ImGui::Image(lit_scene_tex, viewport_size);
        }
        ImGui::End();
        ImGui::PopStyleVar();

        RenderContext ctx = {};
        ctx.command_list = RenderCore::Get().AcquireCommandList();
        ctx.transient_buffer = RenderCore::Get().GetTransientAllocator();
		ctx.profiler = RenderCore::Get().GetGPUProfiler();
        ctx.current_frame_index = RenderCore::Get().GetCurrentBackBufferIndex();
        ctx.current_back_buffer = RenderCore::Get().GetCurrentRenderTarget();
        ctx.debug_value = debug_value;

        // 暫定でここに置いてるけどどこにおこう
        RenderCore::Get().GetASBuilder()->RecordBuildCommands(ctx.command_list);

        GUISystem::Get().EndFrame();

        render_system.Render(registry, ctx);

		ctx.profiler->Flush(ctx.command_list);

        RenderCore::Get().EndFrame();

        auto frame_end = high_resolution_clock::now();
        duration<double, std::milli> elapsed = frame_end - frame_start;

        // 16.66ms より早く終わっていたら、残りの時間分スレッドを休眠させる
        if (elapsed < target_frame_time)
        {
            auto sleep_time = target_frame_time - elapsed;
            std::this_thread::sleep_for(duration_cast<nanoseconds>(sleep_time));
        }
    }

    render_system.Finalize();

    AssetManager::Shutdown();

    RenderCore::Shutdown();

    WindowSystem::Shutdown();

    system("pause");
    return 0;
}

