#include <iostream>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#include <windows.h>


#include "Source/Runtime/D3D12/D3D12Shader.h"
#include "Source/Runtime/D3D12/D3D12PipelineLayout.h"
#include "Source/Runtime/D3D12/D3D12Dynamic.h"
#include "Source/Runtime/D3D12/D3D12Buffer.h"
#include "Source/Runtime/D3D12/D3D12DescriptorHeap.h"
#include "Source/Runtime/D3D12/D3D12CommandBuffer.h"
#include "Source/Runtime/D3D12/D3D12Util.h"
#include "Source/Core/Window/WindowSystem.h"
#include "Source/Runtime/RenderCore/RenderCore.h"
#include "Source/Runtime/RenderCore/RenderHardwareInterface.h"
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
#include "Source/Runtime/Renderer/CameraComponent.h"

#include "RHI/Definitions.h"

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

    {
        /*
        * 独自ファイルフォーマット作ったら消す(独自ファイルフォーマットづくりはいつかね....)
        */
        auto RegisterColorSet = [](const String& mat_uri, const String& table_uri, const MaterialProperties& props) {
            auto mat = std::make_shared<MaterialHandle>();
            mat->SetName(mat_uri);
            mat->GetProperties() = props;
            AssetManager::Get().RegisterAsset(mat_uri, mat);

            auto table = std::make_shared<MaterialTableHandle>();
            table->SetName(table_uri);
            table->AddMaterial(mat);
            AssetManager::Get().RegisterAsset(table_uri, table);
        };

        // --- 赤壁 ---
        MaterialProperties red_props{};
        red_props.base_color = float3(0.63f, 0.06f, 0.04f);
        red_props.roughness = 0.5f;
        RegisterColorSet(L"Alpha://Material/Red", L"Alpha://MaterialTable/Red", red_props);

        // --- 緑壁 ---
        MaterialProperties green_props{};
        green_props.base_color = float3(0.15f, 0.48f, 0.09f);
        green_props.roughness = 0.5f;
        RegisterColorSet(L"Alpha://Material/Green", L"Alpha://MaterialTable/Green", green_props);

        // --- 発光面 ---
        MaterialProperties light_props{};
        light_props.base_color = float3(1.0f, 1.0f, 1.0f);
        light_props.emissive_color = float3(1.0f, 1.0f, 1.0f);
        light_props.emissive_intensity = 15.0f;
        light_props.roughness = 0.4f;
        RegisterColorSet(L"Alpha://Material/Light", L"Alpha://MaterialTable/Light", light_props);
    }

    
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

    UINT32 white_index = RenderResourceManager::Get().GetMaterialTableIndexByMesh(cube_index);
    UInt32 red_index = RenderResourceManager::Get().RegisterMaterialTable(AssetManager::Get().Load<MaterialTableHandle>(L"Alpha://MaterialTable/Red"));
    UInt32 green_index = RenderResourceManager::Get().RegisterMaterialTable(AssetManager::Get().Load<MaterialTableHandle>(L"Alpha://MaterialTable/Green"));
    UInt32 light_index = RenderResourceManager::Get().RegisterMaterialTable(AssetManager::Get().Load<MaterialTableHandle>(L"Alpha://MaterialTable/Light"));


    RenderSystem::Args rs_args = {
        .render_core = RenderCore::GetPtr(),
    };
    RenderSystem::Startup(rs_args);

    RenderCore::Get().PostInitialize();

    entt::registry registry;

    {
        const auto entity = registry.create();

        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = akari_index;
        mesh.material_table_index = RenderResourceManager::Get().GetMaterialTableIndexByMesh(akari_index);

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(10.0f, 0.0f, 0.0f));

    }
    {
        const auto entity = registry.create();

        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = akari_index;
        mesh.material_table_index = RenderResourceManager::Get().GetMaterialTableIndexByMesh(akari_index);

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(10.0f, 0.0f, 10.0f));

    }

    {
        const auto entity = registry.create();

        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = patchouli_index;
        mesh.material_table_index = RenderResourceManager::Get().GetMaterialTableIndexByMesh(patchouli_index);

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(-10.0f, 0.0f, 0.0f));

    }

    {
        const auto entity = registry.create();

        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = sphere_index;
        mesh.material_table_index = RenderResourceManager::Get().GetMaterialTableIndexByMesh(sphere_index);

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(-10.0f, 0.0f, 10.0f));

    }

    {
        const auto entity = registry.create();

        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;
        mesh.material_table_index = white_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(0.0f, -0.5f, 0.0f)); 
        transform.SetLocalScale(Vector3(60.0f, 1.0f, 60.0f));

    }

    // 天井 (Ceiling) - 白
    {
        const auto entity = registry.create();
        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;
        mesh.material_table_index = white_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(0.0f, 60.5f, 0.0f)); // 高さ60
        transform.SetLocalScale(Vector3(60.0f, 1.0f, 60.0f));
    }

    // 奥の壁 (Back Wall) - 白
    {
        const auto entity = registry.create();
        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;
        mesh.material_table_index = white_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(0.0f, 30.0f, 30.5f)); // 奥（Zプラス）
        transform.SetLocalScale(Vector3(60.0f, 60.0f, 1.0f));
    }

    // 左の壁 (Left Wall) - 赤
    {
        const auto entity = registry.create();
        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;
        mesh.material_table_index = red_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(-30.5f, 30.0f, 0.0f)); // 左（Xマイナス）
        transform.SetLocalScale(Vector3(1.0f, 60.0f, 60.0f));
    }

    // 右の壁 (Right Wall) - 緑
    {
        const auto entity = registry.create();
        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;
        mesh.material_table_index = green_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(30.5f, 30.0f, 0.0f)); // 右（Xプラス）
        transform.SetLocalScale(Vector3(1.0f, 60.0f, 60.0f));
    }

    // 天井の中央光源 (Ceiling Light) - 発光
    {
        const auto entity = registry.create();
        auto& mesh = registry.emplace<MeshComponent>(entity);
        mesh.mesh_index = cube_index;
        mesh.material_table_index = light_index;

        auto& transform = registry.emplace<TransformComponent>(entity);
        transform.SetLocalPosition(Vector3(0.0f, 59.9f, 0.0f)); // 天井のほんの少し下
        transform.SetLocalScale(Vector3(15.0f, 0.1f, 15.0f));   // 天井の1/4サイズのエリアライト
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

        RenderContext ctx = {};
        ctx.command_list = RenderCore::Get().GetCommandList();
        ctx.transient_buffer = RenderCore::Get().GetTransientAllocator();
        ctx.current_frame_index = RenderCore::Get().GetCurrentBackBufferIndex();

        RenderSystem::Get().Render(registry, ctx);

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

    RenderSystem::Shutdown();

    AssetManager::Shutdown();

    RenderCore::Shutdown();

    WindowSystem::Shutdown();

    system("pause");
    return 0;
}

