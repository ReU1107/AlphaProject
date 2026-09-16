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

inline void BuildParticleTestScene(
    Scene& scene,
    UInt32 sphere_index,
    UInt32 cube_index)
{
    auto create_solid_mesh = [&scene](
        const String& name,
        UInt32 mesh_index,
        const Vector3& position,
        const Vector3& scale,
        const Color3& color)
    {
        SceneObject& object = scene.CreateObject(name);

        auto& mesh = object.AddComponent<MeshComponent>();
        mesh.mesh_index = mesh_index;

        auto& transform = object.GetComponent<TransformComponent>();
        transform.SetLocalPosition(position);
        transform.SetLocalScale(scale);

        auto& material = object.AddComponent<MaterialOverrideComponent>();
        auto& resource_manager = RenderResourceManager::Get();
        auto& asset_manager = AssetManager::Get();
        material.materials = resource_manager.GetDefaultMaterials(mesh_index);

        // Override は BuildMeshes の ResolveMaterialTextures を通るため、
        // ここでは GPU の SRV index ではなく、CPU のパレット index を設定する。
        // 起動時に登録済みの組み込みテクスチャなので RegisterTexture は既存 ID を返す。
        const UInt32 white_texture_index =
            resource_manager.RegisterTexture(asset_manager.GetWhiteTexture());
        const UInt32 normal_texture_index =
            resource_manager.RegisterTexture(asset_manager.GetNormalTexture());
        const UInt32 black_texture_index =
            resource_manager.RegisterTexture(asset_manager.GetBlackTexture());

        const UInt32 material_count =
            static_cast<UInt32>(material.materials.size());

        for (UInt32 i = 0; i < material_count; ++i)
        {
            auto& data = material.materials[i];
            data.base_color_tex_idx = white_texture_index;
            data.normal_tex_idx = normal_texture_index;
            data.metallic_roughness_tex_idx = white_texture_index;
            data.emissive_tex_idx = black_texture_index;

            material.SetBaseColor(i, color);
            material.SetRoughness(i, 0.9f);
            material.SetMetallic(i, 0.0f);
        }
    };

    auto create_emitter = [&scene](
        const String& name,
        const Vector3& position,
        const float3& velocity,
        const float4& color,
        float emission_rate,
        float lifetime,
        float size)
    {
        SceneObject& object = scene.CreateObject(name);
        object.GetComponent<TransformComponent>().SetLocalPosition(position);

        auto& emitter = object.AddComponent<ParticleEmitterComponent>();
        emitter.emission_rate = emission_rate;
        emitter.lifetime = lifetime;
        emitter.initial_velocity = velocity;
        emitter.velocity_random = float3(0.0f, 0.0f, 0.0f);

        // 初回の描画確認では色・サイズを一定にする。
        // DisabledBlend のパイプラインでも確認できるよう alpha は 1。
        emitter.start_color = color;
        emitter.end_color = color;
        emitter.start_size = size;
        emitter.end_size = size;

        // 最初の更新から発生させるための 1 個分。
        emitter.spawn_accumulator = 1.0f;
    };

    // 単色の床と背景。外部の画像ファイルは使用しない。
    create_solid_mesh(
        L"ParticleTestGround", cube_index,
        Vector3(0.0f, -0.5f, 8.0f),
        Vector3(26.0f, 1.0f, 34.0f),
        Color3(0.12f, 0.14f, 0.17f));

    create_solid_mesh(
        L"ParticleTestBackdrop", cube_index,
        Vector3(0.0f, 4.0f, 16.0f),
        Vector3(22.0f, 8.0f, 0.5f),
        Color3(0.055f, 0.065f, 0.085f));

    // 3 色の台座と、小さな球の発生位置マーカー。
    struct EmitterPreset
    {
        const wchar_t* pedestal_name;
        const wchar_t* marker_name;
        const wchar_t* emitter_name;
        Vector3 position;
        float3 velocity;
        float4 color;
        Color3 pedestal_color;
        float emission_rate;
        float lifetime;
        float size;
    };

    const EmitterPreset presets[] =
    {
        {
            L"WarmPedestal", L"WarmMarker", L"WarmEmitter",
            Vector3(-6.0f, 1.2f, 4.0f),
            float3(0.35f, 1.8f, 0.0f),
            float4(1.0f, 0.35f, 0.08f, 1.0f),
            Color3(0.35f, 0.12f, 0.04f),
            60.0f, 3.0f, 0.30f,
        },
        {
            L"CyanPedestal", L"CyanMarker", L"CyanEmitter",
            Vector3(1.6f, 1.2f, 8.0f),
            float3(-0.65f, 0.9f, 0.0f),
            float4(0.08f, 0.65f, 1.0f, 1.0f),
            Color3(0.04f, 0.22f, 0.35f),
            80.0f, 4.0f, 0.24f,
        },
        {
            L"GreenPedestal", L"GreenMarker", L"GreenEmitter",
            Vector3(6.0f, 1.2f, 4.0f),
            float3(-0.35f, 1.8f, 0.0f),
            float4(0.20f, 1.0f, 0.35f, 1.0f),
            Color3(0.08f, 0.30f, 0.12f),
            60.0f, 3.0f, 0.30f,
        },
    };

    for (const auto& preset : presets)
    {
        // 成分の取得 API を増やさず、元の位置から高さだけを引く。
        create_solid_mesh(
            preset.pedestal_name, cube_index,
            preset.position - Vector3(0.0f, 1.05f, 0.0f),
            Vector3(2.2f, 0.3f, 2.2f),
            preset.pedestal_color);

        create_solid_mesh(
            preset.marker_name, sphere_index,
            preset.position - Vector3(0.0f, 0.5f, 0.0f),
            Vector3(0.25f, 0.25f, 0.25f),
            preset.pedestal_color);

        create_emitter(
            preset.emitter_name,
            preset.position,
            preset.velocity,
            preset.color,
            preset.emission_rate,
            preset.lifetime,
            preset.size);
    }

    // 中央エミッターの手前に置き、粒子と不透明物体の深度関係を確認する。
    create_solid_mesh(
        L"ParticleDepthOccluder", cube_index,
        Vector3(0.0f, 2.0f, 5.0f),
        Vector3(1.2f, 4.0f, 0.6f),
        Color3(0.42f, 0.44f, 0.48f));

    {
        SceneObject& object = scene.CreateObject(L"ParticleTestLight");
        auto& light = object.AddComponent<LightComponent>();
        light.type = 2;
        light.direction = VectorNormalize(Vector3(-0.4f, -1.0f, 0.2f));
        light.SetColor(float3(0.95f, 0.97f, 1.0f));
        light.SetIntensity(3.0f);
    }

    {
        SceneObject& object = scene.CreateObject(L"MainCamera");
        auto& transform = object.GetComponent<TransformComponent>();

        // 添付された Showcase のカメラ設定を使用。
        transform.SetLocalPosition(Vector3(0.0f, 5.0f, -28.0f));
        transform.LookAt(Vector3(0.0f, -0.05f, 1.0f));
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

    BuildParticleTestScene(scene, sphere_index, cube_index);

    //BuildShowcaseScene(
    //    scene,
    //    akari_index,
    //    patchouli_index,
    //    sphere_index,
    //    cube_index,

    //    bricks_index,
    //    concrete_index,
    //    metal_index,
    //    rocks_index);

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

