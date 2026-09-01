#include <iostream>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#include <windows.h>


#include "Source/Core/Window/WindowSystem.h"

#include "Source/Runtime/RenderCore/Module.h"


#include "Source/Runtime/Asset/Module.h"
#include "Source/Runtime/Animation/Module.h"
#include "Source/Runtime/Renderer/Module.h"

#include "Source/Runtime/Scene/Module.h"
#include "Source/Runtime/Particle/Module.h"


#include "Source/Core/Serialization/ByteWriter.h"

#include "ThirdParty/entt/entt.hpp"
#include "ThirdParty/imgui/imgui.h"

#include "Source/Runtime/System/RenderSystem.h"
#include "Source/Runtime/System/CameraSystem.h"
#include "Source/Runtime/System/VertexComputePass.h"
#include "Source/Runtime/System/RenderScene.h"
#include "Source/Runtime/System/VBufferPass.h"
#include "Source/Runtime/System/FroxelLightingPass.h"
#include "Source/Runtime/System/VolumetricPass.h"
#include "Source/Runtime/System/CompositePass.h"
#include "Source/Runtime/Renderer/CameraComponent.h"

#include "Source/Runtime/Spatial/TransformComponent.h"
#include "RHI/Definitions.h"

#include "Source/Core/Math/MathF.h"

#include <thread>
#include <chrono>

#pragma comment(lib, "dxguid.lib")

using Microsoft::WRL::ComPtr;
using namespace Alpha;
using namespace Alpha::Renderer;


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
    
    RenderCore::Args render_core_args = {};
    render_core_args.hWnd = (HWND)WindowSystem::Get().GetNativeHandle();
    render_core_args.transient_buffer_size = 1_MB;
    RenderCore::Startup(render_core_args);

    GUISystem::Args gui_args = {
        .render_core = RenderCore::GetPtr(),
    };
    GUISystem::Startup(gui_args);

    DebugRenderer::Args debug_args = {
        .render_core = RenderCore::GetPtr(),
    };
    DebugRenderer::Startup(debug_args);


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

    ParticleSystem::Args ps_args = {
        .render_core = RenderCore::GetPtr(),
        .max_particle_count = 10000,
    };
    ParticleSystem particle_system;
    particle_system.Initialize(ps_args);

    Scene scene;
    scene.Initialize();

    // 既存処理でRegistryが必要な場合だけ参照を取る
    entt::registry& registry = scene.GetRegistry();

    auto create_mesh_object =
        [&](const String& name,
        UInt32 mesh_index,
        const Vector3& position,
        const Vector3& scale) -> SceneObject&
    {
        SceneObject& object =
            scene.CreateObject(name);

        auto& mesh =
            object.AddComponent<MeshComponent>();

        mesh.mesh_index = mesh_index;

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(position);
        transform.SetLocalScale(scale);

        return object;
    };

    auto add_default_materials =
        [&](SceneObject& object,
        UInt32 mesh_index)
        -> MaterialOverrideComponent&
    {
        auto& material =
            object.AddComponent<
            MaterialOverrideComponent>();

        material.materials =
            RenderResourceManager::Get()
            .GetDefaultMaterials(mesh_index);

        return material;
    };

    const Vector3 default_scale(
        1.0f,
        1.0f,
        1.0f);

    // Akari 1
    create_mesh_object(
        L"Akari_1",
        akari_index,
        Vector3(10.0f, 0.0f, 0.0f),
        default_scale);

    // Akari 2
    create_mesh_object(
        L"Akari_2",
        akari_index,
        Vector3(10.0f, 0.0f, 10.0f),
        default_scale);

    // Patchouli
    create_mesh_object(
        L"Patchouli",
        patchouli_index,
        Vector3(-10.0f, 0.0f, 0.0f),
        default_scale);

    // Sphere
    create_mesh_object(
        L"Sphere",
        sphere_index,
        Vector3(-10.0f, 0.0f, 10.0f),
        default_scale);

    SceneObject* floor_object = nullptr;

    // Floor
    {
        SceneObject& object =
            create_mesh_object(
            L"Floor",
            cube_index,
            Vector3(0.0f, -0.5f, 0.0f),
            Vector3(60.0f, 1.0f, 60.0f));

        add_default_materials(
            object,
            cube_index);

        floor_object = &object;
    }

    // Ceiling
    {
        SceneObject& object =
            create_mesh_object(
            L"Ceiling",
            cube_index,
            Vector3(0.0f, 60.5f, 0.0f),
            Vector3(60.0f, 1.0f, 60.0f));

        add_default_materials(
            object,
            cube_index);
    }

    // Back wall
    {
        SceneObject& object =
            create_mesh_object(
            L"BackWall",
            cube_index,
            Vector3(0.0f, 30.0f, 30.5f),
            Vector3(60.0f, 60.0f, 1.0f));

        add_default_materials(
            object,
            cube_index);
    }

    // Left wall
    {
        SceneObject& object =
            create_mesh_object(
            L"LeftWall",
            cube_index,
            Vector3(-30.5f, 30.0f, 0.0f),
            Vector3(1.0f, 60.0f, 60.0f));

        auto& material =
            add_default_materials(
            object,
            cube_index);

        material.SetBaseColor(
            0,
            Color3(0.63f, 0.06f, 0.04f));
    }

    // Right wall lower
    {
        SceneObject& object =
            create_mesh_object(
            L"RightWallLower",
            cube_index,
            Vector3(30.5f, 10.0f, 0.0f),
            Vector3(1.0f, 20.0f, 60.0f));

        auto& material =
            add_default_materials(
            object,
            cube_index);

        material.SetBaseColor(
            0,
            Color3(0.15f, 0.48f, 0.09f));
    }

    // Right wall upper
    {
        SceneObject& object =
            create_mesh_object(
            L"RightWallUpper",
            cube_index,
            Vector3(30.5f, 50.0f, 0.0f),
            Vector3(1.0f, 20.0f, 60.0f));

        auto& material =
            add_default_materials(
            object,
            cube_index);

        material.SetBaseColor(
            0,
            Color3(0.15f, 0.48f, 0.09f));
    }

    // Right wall back
    {
        SceneObject& object =
            create_mesh_object(
            L"RightWallBack",
            cube_index,
            Vector3(30.5f, 30.0f, -20.0f),
            Vector3(1.0f, 20.0f, 20.0f));

        auto& material =
            add_default_materials(
            object,
            cube_index);

        material.SetBaseColor(
            0,
            Color3(0.15f, 0.48f, 0.09f));
    }

    // Right wall front
    {
        SceneObject& object =
            create_mesh_object(
            L"RightWallFront",
            cube_index,
            Vector3(30.5f, 30.0f, 20.0f),
            Vector3(1.0f, 20.0f, 20.0f));

        auto& material =
            add_default_materials(
            object,
            cube_index);

        material.SetBaseColor(
            0,
            Color3(0.15f, 0.48f, 0.09f));
    }

    SceneObject* directional_light_object = nullptr;

    // Directional light
    {
        SceneObject& object =
            scene.CreateObject(
            L"DirectionalLight");

        auto& light =
            object.AddComponent<LightComponent>();

        light.type = 2;
        light.direction =
            VectorNormalize(
            Vector3(-1.0f, -0.5f, 0.0f));

        light.SetColor(
            float3(1.0f, 0.95f, 0.8f));

        light.SetIntensity(1.0f);

        directional_light_object = &object;
    }

    // Point light
    {
        SceneObject& object =
            scene.CreateObject(
            L"PointLight");

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(
            Vector3(0.0f, 50.0f, 0.0f));

        auto& light =
            object.AddComponent<LightComponent>();

        light.type = 0;
        light.SetColor(
            float3(1.0f, 0.0f, 0.85f));

        light.SetIntensity(5.0f);
        light.SetRadius(45.0f);
    }

    // Spot light
    {
        SceneObject& object =
            scene.CreateObject(
            L"SpotLight");

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(
            Vector3(-10.0f, 20.0f, 10.0f));

        transform.LookAt(
            Vector3(0.0f, -1.0f, -1.0f));

        auto& light =
            object.AddComponent<LightComponent>();

        light.type = 1;
        light.SetColor(
            float3(0.2f, 0.6f, 1.0f));

        light.SetIntensity(12.0f);
        light.SetRadius(60.0f);

        light.SetSpotAngleDegree(
            30.0f,
            60.0f);
    }

    // Camera
    {
        SceneObject& object =
            scene.CreateObject(
            L"MainCamera");

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(
            Vector3(0.0f, 20.0f, -20.0f));

        object.AddComponent<CameraComponent>();
    }

    // Fog volume
    {
        SceneObject& object =
            scene.CreateObject(
            L"FogVolume");

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(
            Vector3(0.0f, 25.0f, 0.0f));

        auto& fog =
            object.AddComponent<VolumeComponent>();

        fog.SetSize(
            Vector3(45.0f, 45.0f, 45.0f));

        fog.SetDensity(1.5f);
    }

    {
        SceneObject& object = scene.CreateObject(L"Emitter");

        auto& transform = object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(Vector3(0.0f, 25.0f, 0.0f));

        auto& fog = object.AddComponent<ParticleEmitterComponent>();
        
    }


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

        ParticleContext pctx = {};
        pctx.command_list = RenderCore::Get().AcquireCommandList();
        pctx.current_frame_index = RenderCore::Get().GetCurrentBackBufferIndex();
        pctx.transient_buffer = RenderCore::Get().GetTransientAllocator();
        pctx.delta_time = 1.0f / 60;

        particle_system.Update(registry, pctx);


        RenderContext ctx = {};
        ctx.command_list = RenderCore::Get().AcquireCommandList();
        ctx.transient_buffer = RenderCore::Get().GetTransientAllocator();
        ctx.current_frame_index = RenderCore::Get().GetCurrentBackBufferIndex();

        // 暫定でここに置いてるけどどこにおこう
        RenderCore::Get().GetASBuilder()->RecordBuildCommands(ctx.command_list);

        render_system.Render(registry, ctx);


        GUISystem::Get().EndFrame();

        auto list = RenderCore::Get().AcquireCommandList();

        UInt32 width = RenderCore::Get().GetWidth();
        UInt32 height = RenderCore::Get().GetHeight();

        list->Transition(RenderCore::Get().GetCurrentRenderTarget(), ERHIResourceState::RenderTarget);
        list->Transition(RenderCore::Get().GetDepthTexture(), ERHIResourceState::DepthWrite);

        list->SetRenderTarget(RenderCore::Get().GetCurrentRenderTarget(), RenderCore::Get().GetDepthTexture());
        list->ClearRenderTarget(RenderCore::Get().GetCurrentRenderTarget(), Rect2D(0, 0, width, height), Color4::Gray);
        list->ClearDepthStencil(RenderCore::Get().GetDepthTexture(), Rect2D(0, 0, width, height), 0.0f);
        list->SetViewport(Viewport(0, 0, (float)width, (float)height, 0.0f, 1.0f));

        list->SetScissorRect(0, 0, width, height);
    
        ctx.command_list = list;
        GUISystem::Get().Flush(ctx);

        list->Transition(RenderCore::Get().GetDepthTexture(), ERHIResourceState::Common);
        list->Transition(RenderCore::Get().GetCurrentRenderTarget(), ERHIResourceState::Present);

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

