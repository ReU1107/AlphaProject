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

#include "Source/Core/Serialization/ByteWriter.h"

#include "ThirdParty/entt/entt.hpp"
#include "ThirdParty/imgui/imgui.h"

#include "Source/Runtime/System/RenderSystem.h"
#include "Source/Runtime/System/TransformComponent.h"
#include "Source/Runtime/System/CameraSystem.h"
#include "Source/Runtime/System/VertexComputePass.h"
#include "Source/Runtime/System/RenderScene.h"
#include "Source/Runtime/System/VBufferPass.h"
#include "Source/Runtime/Renderer/CameraComponent.h"

#include "RHI/Definitions.h"

#include <thread>
#include <chrono>

#pragma comment(lib, "dxguid.lib")

using Microsoft::WRL::ComPtr;
using namespace Alpha;
using namespace Alpha::Renderer;

static void RenderDebugUI(entt::registry& registry, entt::entity floor,UInt32 cube_index)
{
    // 生成された床のエンティティが有効か一応チェック
    if (floor == entt::null || !registry.valid(floor))
    {
        return;
    }

    ImGui::Begin("床のマテリアル変更デバッグ窓");
    {
        // すでに上書きコンポーネントを持っているかチェック
        auto* override_com = registry.try_get<MaterialOverrideComponent>(floor);

        if (!override_com)
        {
            if (ImGui::Button("マテリアル上書きを有効化"))
            {
                // ボタンを押した瞬間にコンポーネントを付与してデフォルト値で初期化！
                auto& new_com = registry.emplace<MaterialOverrideComponent>(floor);
                new_com.materials = RenderResourceManager::Get().GetDefaultMaterials(cube_index);
            }
            ImGui::Text("現在はメッシュ本来のデフォルトマテリアルで描画中");
        }
        else
        {
            if (ImGui::Button("上書きを解除してデフォルトに戻す"))
            {
                // コンポーネントを消去（これでGPU側には自動で 0xFFFFFFFF が送られる）
                registry.remove<MaterialOverrideComponent>(floor);
                ImGui::End();
                return;
            }

            ImGui::Separator();

            // 床のマテリアル配列を回して色を編集（床が単一マテリアルなら0番目だけでOKですが一応ループで）
            for (int i = 0; i < static_cast<int>(override_com->materials.size()); ++i)
            {
                ImGui::PushID(i);

                auto& current_prop = override_com->materials[i].props;

                // ImGui用のバッファに現在の色をセット
                float color_buffer[3] = { current_prop.base_color.X(), current_prop.base_color.Y(), current_prop.base_color.Z() };

                if (ImGui::ColorEdit3("Base Color", color_buffer))
                {
                    // カラーピッカーが動いたら、先ほど作った Setter で安全に上書き
                    Color3 new_color{ color_buffer[0], color_buffer[1], color_buffer[2] };
                    override_com->SetBaseColor(i, new_color);
                }

                // 一緒に金属感や粗さも変えられるようにしておくとデバッグが捗ります
                ImGui::SliderFloat("Roughness", &current_prop.roughness, 0.0f, 1.0f);
                ImGui::SliderFloat("Metallic", &current_prop.metallic, 0.0f, 1.0f);

                ImGui::PopID();
            }
        }
    }
    ImGui::End();
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
    
    RenderCore::Args render_core_args = {};
    render_core_args.hWnd = (HWND)WindowSystem::Get().GetNativeHandle();
    render_core_args.transient_buffer_size = 1_MB;
    RenderCore::Startup(render_core_args);

    GUISystem::Args gui_args = {
        .render_core = RenderCore::GetPtr(),
    };
    GUISystem::Startup(gui_args);

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

    RenderCore::Get().PostInitialize();

    entt::registry registry;

    {
        const auto entity = registry.create();

        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = akari_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(10.0f, 0.0f, 0.0f));

    }
    {
        const auto entity = registry.create();

        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = akari_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(10.0f, 0.0f, 10.0f));

    }

    {
        const auto entity = registry.create();

        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = patchouli_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(-10.0f, 0.0f, 0.0f));

    }

    {
        const auto entity = registry.create();

        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = sphere_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(-10.0f, 0.0f, 10.0f));

    }

    entt::entity floor = entt::null;

    {
        const auto entity = registry.create();

        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(0.0f, -0.5f, 0.0f)); 
        transform.SetLocalScale(Vector3(60.0f, 1.0f, 60.0f));

        auto& ma = registry.emplace<MaterialOverrideComponent>(entity);
        ma.materials = RenderResourceManager::Get().GetDefaultMaterials(cube_index);

        floor = entity;
    }

    {
        const auto entity = registry.create();
        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(0.0f, 60.5f, 0.0f)); // 高さ60
        transform.SetLocalScale(Vector3(60.0f, 1.0f, 60.0f));
        auto& ma = registry.emplace<MaterialOverrideComponent>(entity);
        ma.materials = RenderResourceManager::Get().GetDefaultMaterials(cube_index);
    }

    // 奥の壁 (Back Wall) - 白
    {
        const auto entity = registry.create();
        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(0.0f, 30.0f, 30.5f)); // 奥（Zプラス）
        transform.SetLocalScale(Vector3(60.0f, 60.0f, 1.0f));
        auto& ma = registry.emplace<MaterialOverrideComponent>(entity);
        ma.materials = RenderResourceManager::Get().GetDefaultMaterials(cube_index);
    }

    // 左の壁 (Left Wall) - 赤
    {
        const auto entity = registry.create();
        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(-30.5f, 30.0f, 0.0f)); // 左（Xマイナス）
        transform.SetLocalScale(Vector3(1.0f, 60.0f, 60.0f));
        auto& ma = registry.emplace<MaterialOverrideComponent>(entity);
        ma.materials = RenderResourceManager::Get().GetDefaultMaterials(cube_index);
		ma.SetBaseColor(0, Color3(0.63f, 0.06f, 0.04f));
    }

    // 右の壁 (Right Wall) - 緑
    {
        const auto entity = registry.create();
        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(30.5f, 30.0f, 0.0f)); // 右（Xプラス）
        transform.SetLocalScale(Vector3(1.0f, 60.0f, 60.0f));
        auto& ma = registry.emplace<MaterialOverrideComponent>(entity);
        ma.materials = RenderResourceManager::Get().GetDefaultMaterials(cube_index);
        ma.SetBaseColor(0, Color3(0.15f, 0.48f, 0.09f));

    }

    // 天井の中央光源 (Ceiling Light) - 発光
    {
        const auto entity = registry.create();
        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(0.0f, 59.9f, 0.0f)); // 天井のほんの少し下
        transform.SetLocalScale(Vector3(15.0f, 0.1f, 15.0f));   // 天井の1/4サイズのエリアライト
        auto& ma = registry.emplace<MaterialOverrideComponent>(entity);
        ma.materials = RenderResourceManager::Get().GetDefaultMaterials(cube_index);
        ma.SetBaseColor(0, Color3(1.0f, 1.0f, 1.0f));
        ma.SetEmissiveColor(0, Color3(1.0f, 1.0f, 1.0f));
        ma.SetEmissiveIntensity(0, 15.0f);

    }

    // カメラ
    {
        const auto entity = registry.create();
        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(0.0f, 20.0f, -20.0f));

        auto& camera = registry.emplace<CameraComponent>(entity);

    }


    using namespace std::chrono;

    // 60FPSの1フレームあたりの時間（約16.66ms）
    const duration<double, std::milli> target_frame_time(1000.0 / 60.0);

    CameraSystem cm_sys;

    while (WindowSystem::Get().ProcessMessages() && !WindowSystem::Get().ShouldClose())
    {
        auto frame_start = high_resolution_clock::now();

        cm_sys.Update(registry);

        RenderCore::Get().BeginFrame();
        GUISystem::Get().BeginFrame();


        RenderContext ctx = {};
        ctx.command_list = RenderCore::Get().GetCommandList();
        ctx.transient_buffer = RenderCore::Get().GetTransientAllocator();
        ctx.current_frame_index = RenderCore::Get().GetCurrentBackBufferIndex();

        render_system.Render(registry, ctx);


        RenderDebugUI(registry, floor, cube_index);


		GUISystem::Get().EndFrame();

		GUISystem::Get().Flush(ctx);


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

