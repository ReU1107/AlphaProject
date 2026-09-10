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

    auto texture1 = AssetManager::Get().Load<TextureHandle>(L"C:/Users/owner/Desktop/Asset/church_bricks_03_4k.blend/textures/church_bricks_03_diff_4k.jpg");
	texture1->SetName(L"church_bricks_03_diff_4k");
    auto texture2 = AssetManager::Get().Load<TextureHandle>(L"C:/Users/owner/Desktop/Asset/concrete_floor_4k.blend/textures/concrete_floor_diff_4k.jpg");
	texture2->SetName(L"concrete_floor_diff_4k");
    auto texture3 = AssetManager::Get().Load<TextureHandle>(L"C:/Users/owner/Desktop/Asset/metal_plate_02_4k.blend/textures/metal_plate_02_diff_4k.jpg");
    texture3->SetName(L"metal_plate_02_4k");
    auto texture4 = AssetManager::Get().Load<TextureHandle>(L"C:/Users/owner/Desktop/Asset/rocks_ground_06_4k.blend/textures/rocks_ground_06_diff_4k.jpg");
    texture4->SetName(L"rocks_ground_06_4k");

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

    UInt32 bricks_index = RenderResourceManager::Get().RegisterTexture(texture1);
    UInt32 concrete_index = RenderResourceManager::Get().RegisterTexture(texture2);
    UInt32 metal_index = RenderResourceManager::Get().RegisterTexture(texture3);
    UInt32 rocks_index = RenderResourceManager::Get().RegisterTexture(texture4);

    RenderSystem::Args rs_args = {
        .render_core = RenderCore::GetPtr(),
    };
	RenderSystem render_system;
	render_system.Initialize(rs_args);

   /* ParticleSystem::Args ps_args = {
        .render_core = RenderCore::GetPtr(),
        .max_particle_count = 10000,
    };
    ParticleSystem particle_system;
    particle_system.Initialize(ps_args);*/

    Scene scene;
    scene.Initialize();

    BuildShowcaseScene(
        scene,
        akari_index,
        patchouli_index,
        sphere_index,
        cube_index,

        bricks_index,
        concrete_index,
        metal_index,
        rocks_index);

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

        //ParticleContext pctx = {};
        //pctx.command_list = RenderCore::Get().AcquireCommandList();
        //pctx.current_frame_index = RenderCore::Get().GetCurrentBackBufferIndex();
        //pctx.transient_buffer = RenderCore::Get().GetTransientAllocator();
        //pctx.delta_time = 1.0f / 60;

        //particle_system.Update(registry, pctx);

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

