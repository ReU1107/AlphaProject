#include <iostream>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#include <windows.h>


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

#include <thread>
#include <chrono>

#pragma comment(lib, "dxguid.lib")

using Microsoft::WRL::ComPtr;
using namespace Alpha;
using namespace Alpha::Renderer;
using namespace Alpha::Particle;


inline void BuildParticleShapeTestScene(
    Scene& scene,
    UInt32 akari_index,
    UInt32 patchouli_index,
    UInt32 sphere_index,
    UInt32 cube_index)
{
    using Emitter = ::Alpha::Particle::ParticleEmitterComponent;
    using Shape = ::Alpha::Particle::SourceShape;
    using Distribution = ::Alpha::Particle::EmitDistribution;
    using Mesh = ::Alpha::Renderer::MeshComponent;

    constexpr UInt32 invalid_index = 0xFFFFFFFFu;

    // 形状の分布を確認するときは false。
    constexpr bool move_particles = false;

    // false でも MeshComponent を付け、RenderScene と頂点更新に参加させる。
    // true にすると同じ位置に生成元の球・立方体も表示する。
    constexpr bool show_source_meshes = false;

    // キャラクターは元のマテリアルで表示する。false なら粒子だけで形を確認する。
    // 非表示でもメッシュのインスタンス登録と頂点更新は行う。
    constexpr bool show_character_meshes = false;
    constexpr float character_scale = 0.7f;
    constexpr float character_emission_rate = 300.0f;

    // 現在提示された EmitCS には Rectangle / Line の case がないため既定では除外。
    // 対応後に true にすると、左右端に確認用エミッターを追加する。
    constexpr bool include_rectangle_and_line = false;

    auto& resources = RenderResourceManager::Get();
    auto& assets = AssetManager::Get();

    // MaterialOverride の入力には CPU 側のテクスチャパレット index を使う。
    const UInt32 white_texture = resources.RegisterTexture(assets.GetWhiteTexture());
    const UInt32 normal_texture = resources.RegisterTexture(assets.GetNormalTexture());
    const UInt32 black_texture = resources.RegisterTexture(assets.GetBlackTexture());

    auto create_solid_mesh = [&](
        const wchar_t* name,
        const Vector3& position,
        const Vector3& scale,
        const Color3& color)
    {
        SceneObject& object = scene.CreateObject(name);
        object.AddComponent<Mesh>().mesh_index = cube_index;

        auto& transform = object.GetComponent<TransformComponent>();
        transform.SetLocalPosition(position);
        transform.SetLocalScale(scale);

        auto& material = object.AddComponent<MaterialOverrideComponent>();
        material.materials = resources.GetDefaultMaterials(cube_index);

        for (UInt32 i = 0; i < static_cast<UInt32>(material.materials.size()); ++i)
        {
            auto& data = material.materials[i];
            data.base_color_tex_idx = white_texture;
            data.normal_tex_idx = normal_texture;
            data.metallic_roughness_tex_idx = white_texture;
            data.emissive_tex_idx = black_texture;

            material.SetBaseColor(i, color);
            material.SetRoughness(i, 0.9f);
            material.SetMetallic(i, 0.0f);
        }
    };

    create_solid_mesh(
        L"ShapeTest_Ground",
        Vector3(0.0f, -0.35f, 1.0f),
        Vector3(72.0f, 0.6f, 14.0f),
        Color3(0.055f, 0.065f, 0.085f));

    create_solid_mesh(
        L"ShapeTest_Backdrop",
        Vector3(0.0f, 8.5f, 5.0f),
        Vector3(71.0f, 19.0f, 0.3f),
        Color3(0.012f, 0.018f, 0.028f));

    struct Preset
    {
        const wchar_t* emitter_name;
        const wchar_t* pedestal_name;
        Shape shape;
        Distribution distribution;
        UInt32 column;
        UInt32 row;
        UInt32 source_mesh;
        Vector3 scale;
        float emission_rate;
        float particle_size;
    };

    // 上段=Surface、中央=Volume、下段=Point/Circle/Mesh Vertex。
    // Cube Vertex は8隅を確認できる程度に発生数を下げる。
    const Preset presets[] =
    {
        { L"01_Box_Surface", L"01_Pedestal", Shape::Box, Distribution::Surface,
            0, 0, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 300.0f, 0.045f },
        { L"02_Sphere_Surface", L"02_Pedestal", Shape::Sphere, Distribution::Surface,
            1, 0, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 300.0f, 0.045f },
        { L"03_Hemisphere_Surface", L"03_Pedestal", Shape::Hemisphere, Distribution::Surface,
            2, 0, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 300.0f, 0.045f },
        { L"04_Cylinder_Surface", L"04_Pedestal", Shape::Cylinder, Distribution::Surface,
            3, 0, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 300.0f, 0.045f },
        { L"05_MeshSphere_Surface", L"05_Pedestal", Shape::Mesh, Distribution::Surface,
            4, 0, sphere_index, Vector3(1.4f, 1.4f, 1.4f), 300.0f, 0.045f },

        { L"06_Box_Volume", L"06_Pedestal", Shape::Box, Distribution::Volume,
            0, 1, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 300.0f, 0.045f },
        { L"07_Sphere_Volume", L"07_Pedestal", Shape::Sphere, Distribution::Volume,
            1, 1, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 300.0f, 0.045f },
        { L"08_Hemisphere_Volume", L"08_Pedestal", Shape::Hemisphere, Distribution::Volume,
            2, 1, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 300.0f, 0.045f },
        { L"09_Cylinder_Volume", L"09_Pedestal", Shape::Cylinder, Distribution::Volume,
            3, 1, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 300.0f, 0.045f },
        { L"10_Cone_Volume", L"10_Pedestal", Shape::Cone, Distribution::Volume,
            4, 1, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 300.0f, 0.045f },

        { L"11_Point_Center", L"11_Pedestal", Shape::Point, Distribution::Center,
            0, 2, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 8.0f, 0.10f },
        { L"12_Circle_Surface", L"12_Pedestal", Shape::Circle, Distribution::Surface,
            1, 2, invalid_index, Vector3(1.0f, 1.0f, 1.0f), 200.0f, 0.045f },
        { L"13_MeshSphereA_Vertex", L"13_Pedestal", Shape::Mesh, Distribution::Vertex,
            2, 2, sphere_index, Vector3(1.4f, 1.4f, 1.4f), 300.0f, 0.055f },
        { L"14_MeshSphereB_Vertex", L"14_Pedestal", Shape::Mesh, Distribution::Vertex,
            3, 2, sphere_index, Vector3(0.9f, 1.7f, 1.2f), 300.0f, 0.055f },
        { L"15_MeshCube_Vertex", L"15_Pedestal", Shape::Mesh, Distribution::Vertex,
            4, 2, cube_index, Vector3(1.6f, 1.6f, 1.6f), 40.0f, 0.085f },
    };

    const float4 row_colors[] =
    {
        float4(1.0f, 0.48f, 0.08f, 1.0f),
        float4(0.08f, 0.65f, 1.0f, 1.0f),
        float4(0.30f, 1.0f, 0.45f, 1.0f),
    };
    const Color3 pedestal_colors[] =
    {
        Color3(0.18f, 0.065f, 0.015f),
        Color3(0.015f, 0.075f, 0.18f),
        Color3(0.025f, 0.14f, 0.045f),
    };

    auto configure_emitter = [&](Emitter& emitter, Shape shape,
        Distribution distribution, float rate, float size,
        const float4& color, UInt32 seed)
    {
        emitter.source_shape = shape;
        emitter.distribution = distribution;

        emitter.max_particle_count = 1000;
        emitter.emission_rate = rate;
        emitter.lifetime = 2.0f;
        emitter.duration = 3600.0f;

        emitter.initial_velocity = move_particles
            ? float3(0.0f, 0.4f, 0.0f) : float3(0.0f, 0.0f, 0.0f);
        emitter.velocity_random = move_particles
            ? float3(0.12f, 0.12f, 0.12f) : float3(0.0f, 0.0f, 0.0f);

        emitter.start_color = color;
        emitter.end_color = color;
        emitter.start_size = size;
        emitter.end_size = size;
        emitter.random_seed = seed;
        emitter.spawn_accumulator = 0.0f;

        emitter.box_half_size = float3(1.2f, 1.0f, 0.9f);
        emitter.radius = 1.25f;
        emitter.height = 2.5f;
        emitter.rectangle_half_size = float2(1.4f, 0.8f);
        emitter.line_length = 2.8f;

        emitter.Start();
    };

    UInt32 preset_index = 0;
    for (const auto& preset : presets)
    {
        const float x = -12.0f + 6.0f * static_cast<float>(preset.column);
        const float y = 14.4f - 5.8f * static_cast<float>(preset.row);
        const Vector3 position(x, y, 0.0f);

        create_solid_mesh(
            preset.pedestal_name,
            Vector3(x, y - 2.3f, 0.0f),
            Vector3(3.6f, 0.14f, 3.6f),
            pedestal_colors[preset.row]);

        SceneObject& object = scene.CreateObject(preset.emitter_name);
        auto& transform = object.GetComponent<TransformComponent>();
        transform.SetLocalPosition(position);
        transform.SetLocalScale(preset.scale);

        if (preset.shape == Shape::Mesh)
        {
            // エミッターと同じ Entity に付ける。
            // 基本形状には MeshComponent を付けない。
            auto& mesh = object.AddComponent<Mesh>();
            mesh.mesh_index = preset.source_mesh;
            mesh.visible = show_source_meshes;
        }

        auto& emitter = object.AddComponent<Emitter>();
        configure_emitter(
            emitter, preset.shape, preset.distribution,
            preset.emission_rate, preset.particle_size,
            row_colors[preset.row], 1001u + preset_index * 97u);

        ++preset_index;
    }

    if (include_rectangle_and_line)
    {
        const Shape pending_shapes[] = { Shape::Rectangle, Shape::Line };
        const Distribution pending_distributions[] =
        {
            Distribution::Surface,
            Distribution::Edge,
        };
        const wchar_t* pending_names[] =
        {
            L"16_Rectangle_Surface",
            L"17_Line_Edge",
        };
        const wchar_t* pending_pedestals[] =
        {
            L"16_Pedestal",
            L"17_Pedestal",
        };

        for (UInt32 i = 0; i < 2; ++i)
        {
            const float x = i == 0 ? -18.0f : 18.0f;
            create_solid_mesh(
                pending_pedestals[i], Vector3(x, 0.5f, 0.0f),
                Vector3(3.6f, 0.14f, 3.6f), Color3(0.12f, 0.035f, 0.15f));

            SceneObject& object = scene.CreateObject(pending_names[i]);
            auto& transform = object.GetComponent<TransformComponent>();
            transform.SetLocalPosition(Vector3(x, 2.8f, 0.0f));
            transform.SetLocalScale(Vector3(1.0f, 1.0f, 1.0f));

            auto& emitter = object.AddComponent<Emitter>();
            configure_emitter(
                emitter, pending_shapes[i], pending_distributions[i],
                i == 0 ? 200.0f : 80.0f, 0.055f,
                float4(0.85f, 0.22f, 1.0f, 1.0f), 7001u + i * 97u);
        }
    }

    // 既存の形状一覧の左右に、モデルの頂点から生成する枠を追加する。
    struct CharacterPreset
    {
        const wchar_t* emitter_name;
        const wchar_t* pedestal_name;
        UInt32 mesh_index;
        float x;
        Distribution distribution;
        float4 particle_color;
        Color3 pedestal_color;
        UInt32 random_seed;
    };

    const CharacterPreset characters[] =
    {
        {
            L"18_Akari_Vertex", L"18_Akari_Pedestal", akari_index, -26.0f,
            Distribution::Vertex,
            float4(1.0f, 0.30f, 0.06f, 1.0f),
            Color3(0.18f, 0.05f, 0.015f), 8101u,
        },
        {
            L"19_Patchouli_Vertex", L"19_Patchouli_Pedestal", patchouli_index, 26.0f,
            Distribution::Vertex,
            float4(0.75f, 0.25f, 1.0f, 1.0f),
            Color3(0.10f, 0.025f, 0.18f), 8201u,
        },
    };

    for (const auto& preset : characters)
    {
        create_solid_mesh(
            preset.pedestal_name,
            Vector3(preset.x, 0.25f, 0.0f),
            Vector3(8.0f, 0.5f, 8.0f),
            preset.pedestal_color);

        SceneObject& object = scene.CreateObject(preset.emitter_name);
        auto& transform = object.GetComponent<TransformComponent>();
        transform.SetLocalPosition(Vector3(preset.x, 0.5f, 0.0f));
        transform.SetLocalScale(Vector3(character_scale, character_scale, character_scale));

        auto& mesh = object.AddComponent<Mesh>();
        mesh.mesh_index = preset.mesh_index;
        mesh.visible = show_character_meshes;

        // MaterialOverrideComponent を付けず、登録済みPMXのマテリアルを使用する。
        auto& emitter = object.AddComponent<Emitter>();
        configure_emitter(
            emitter, Shape::Mesh, preset.distribution,
            character_emission_rate, 0.065f,
            preset.particle_color, preset.random_seed);
    }

    {
        SceneObject& object = scene.CreateObject(L"ShapeTest_Light");
        auto& light = object.AddComponent<LightComponent>();
        light.type = 2;
        light.direction = VectorNormalize(Vector3(-0.3f, -1.0f, 0.4f));
        light.SetColor(float3(0.95f, 0.97f, 1.0f));
        light.SetIntensity(3.0f);
    }

    {
        SceneObject& object = scene.CreateObject(L"MainCamera");
        auto& transform = object.GetComponent<TransformComponent>();
        transform.SetLocalPosition(Vector3(0.0f, 8.8f, -54.0f));
        // 以前のシーンと同じ、方向を渡す LookAt の使い方を継承する。
        transform.LookAt(Vector3(0.0f, -0.023f, 1.0f));
        object.AddComponent<CameraComponent>();
    }
}




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

 //   auto texture1 = AssetManager::Get().Load<TextureHandle>(L"C:/Users/owner/Desktop/Asset/church_bricks_03_4k.blend/textures/church_bricks_03_diff_4k.jpg");
	//texture1->SetName(L"church_bricks_03_diff_4k");
 //   auto texture2 = AssetManager::Get().Load<TextureHandle>(L"C:/Users/owner/Desktop/Asset/concrete_floor_4k.blend/textures/concrete_floor_diff_4k.jpg");
	//texture2->SetName(L"concrete_floor_diff_4k");
 //   auto texture3 = AssetManager::Get().Load<TextureHandle>(L"C:/Users/owner/Desktop/Asset/metal_plate_02_4k.blend/textures/metal_plate_02_diff_4k.jpg");
 //   texture3->SetName(L"metal_plate_02_4k");
 //   auto texture4 = AssetManager::Get().Load<TextureHandle>(L"C:/Users/owner/Desktop/Asset/rocks_ground_06_4k.blend/textures/rocks_ground_06_diff_4k.jpg");
 //   texture4->SetName(L"rocks_ground_06_4k");

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

    //UInt32 bricks_index = RenderResourceManager::Get().RegisterTexture(texture1);
    //UInt32 concrete_index = RenderResourceManager::Get().RegisterTexture(texture2);
    //UInt32 metal_index = RenderResourceManager::Get().RegisterTexture(texture3);
    //UInt32 rocks_index = RenderResourceManager::Get().RegisterTexture(texture4);

    RenderSystem::Args rs_args = {
        .render_core = RenderCore::GetPtr(),
    };
	RenderSystem render_system;
	render_system.Initialize(rs_args);


    Scene scene;
    scene.Initialize();

    BuildParticleShapeTestScene(
        scene,
        akari_index,
        patchouli_index,
        sphere_index,
        cube_index);
    auto& registry = scene.GetRegistry();

    using namespace std::chrono;

    // 60FPSの1フレームあたりの時間（約16.66ms）
    const duration<double, std::milli> target_frame_time(1000.0 / 60.0);

    CameraSystem cm_sys;

    while (WindowSystem::Get().ProcessMessages() && !WindowSystem::Get().ShouldClose())
    {
        auto frame_start = high_resolution_clock::now();

        scene.Update(1.0f / 60);

        cm_sys.Update(registry);

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
        static float pos[3] = { 0.0f, 0.0f, 0.0f };
        ImGui::DragFloat3("Position", pos, 0.1f);
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
        ctx.current_frame_index = RenderCore::Get().GetCurrentBackBufferIndex();
        ctx.current_back_buffer = RenderCore::Get().GetCurrentRenderTarget();

        // 暫定でここに置いてるけどどこにおこう
        RenderCore::Get().GetASBuilder()->RecordBuildCommands(ctx.command_list);

        GUISystem::Get().EndFrame();

        render_system.Render(registry, ctx);



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

