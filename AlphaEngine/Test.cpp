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

#include "Source/Core/Math/MathF.h"

#include "Source/Runtime/System/TransformComponent.h"

#include "RHI/Definitions.h"

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
    auto model3 = AssetManager::Get().GetCube();
    
    RenderCore::Args render_core_args = {};
    render_core_args.hWnd = (HWND)WindowSystem::Get().GetNativeHandle();
    render_core_args.transient_buffer_size = 1_MB;
    RenderCore::Startup(render_core_args);

    auto d = static_cast<D3D12Dynamic*>(RenderCore::Get().GetRHI()->GetDynamic());

    auto device = d->GetDevice()->GetDevice();


    GUISystem::Args gui_args = {
        .render_core = RenderCore::GetPtr(),
    };
    GUISystem::Startup(gui_args);

    RenderResourceManager::Args resource_args = {
        .render_core = RenderCore::GetPtr(),
    };
    RenderResourceManager::Startup(resource_args);

    UInt32 akari_index = RenderResourceManager::Get().RegisterMesh(model1);
    UInt32 patchouli_index = RenderResourceManager::Get().RegisterMesh(model2);
    UInt32 sphere_index = RenderResourceManager::Get().RegisterMesh(model3);

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


    auto as = RenderCore::Get().CreateShader(L"Asset/TaskShader.hlsl", ERHIShaderType::Amplification);
    auto ms = RenderCore::Get().CreateShader(L"Asset/TestMeshShader.hlsl", ERHIShaderType::Mesh);
    auto ps = RenderCore::Get().CreateShader(L"Asset/PixelShader.hlsl", ERHIShaderType::Pixel);

    auto layout = RenderCore::Get().CreatePipelineLayout(
        [](PipelineLayout& layout) 
        {
            layout.AddRootCBV(1, ERHIShaderFlags::All);
			layout.AddRootCBV(1, ERHIShaderFlags::All);
            layout.AddSampler(1, ERHIShaderFlags::Pixel);
            layout.SetFlags(ERHIPipelineLayoutFlags::Bindless);
        }
    );

    auto pipeline = RenderCore::Get().CreateGraphicsPipeline(
        {
            .pipeline_layout = layout.get(),
            .task_shader = as.get(),
            .mesh_shader = ms.get(),
            .pixel_shader = ps.get(),
            .render_target_count = 1,
            .render_targets = {
                RenderCore::Get().GetBackBufferFormat()
            },
            .blend_states = {
                DefaultAlphaBlend()
            },
            .depth_enable = true,

        }
    );

    // インスタンスデータ用
    auto m_instance_allocator = std::make_unique<TransientBufferAllocator>();
    {
        TransientBufferAllocator::Args desc = {
            .render_core = RenderCore::GetPtr(),
            .stride = sizeof(Renderer::InstanceData),
            .size = 4_MB,
            .type = EBufferPresetType::UploadStructured,
        };
        m_instance_allocator->Initialize(desc);
    }
    TArray<Renderer::InstanceData> instances{};
    instances.reserve(10000);

    struct BufferIndices
    {
        RenderResourceManager::BufferIndices render_resourcce{};
        UInt32 instance{};
    };

    BufferIndices indices = {
        .render_resourcce = RenderResourceManager::Get().GetBufferIndices(),
        .instance = m_instance_allocator->GetBuffer()->GetSRVHandle().index,
    };

    struct Constants
    {
        float4x4 view_proj{ float4x4::Identity };
        float4 camera_pos;
        float4 frustum_planes;

        UInt32 instance_num;
    };

    static float3 camera_pos = Vector3(0.0f, 10.0f, -25.0f);

    auto list = RenderCore::Get().GetCommandList();
    while (WindowSystem::Get().ProcessMessages() && !WindowSystem::Get().ShouldClose())
    {
        RenderCore::Get().BeginFrame();
        list->SetDescriptorHeap(RenderCore::Get().GetResourceHeap(), RenderCore::Get().GetSamplerHeap());

        list->SetGraphicsPipeline(pipeline.get());
        auto native_list = static_cast<D3D12CommandBuffer*>(list->GetRHI())->GetNative();

        RenderContext ctx = {};
        ctx.command_list = RenderCore::Get().GetCommandList();
        ctx.transient_buffer = RenderCore::Get().GetTransientAllocator();
        ctx.current_frame_index = RenderCore::Get().GetCurrentBackBufferIndex();



        if (GetKeyState('W') & 0x8000) { camera_pos.Z() += 0.2f; }
        if (GetKeyState('S') & 0x8000) { camera_pos.Z() -= 0.2f; }
        if (GetKeyState('A') & 0x8000) { camera_pos.X() -= 0.2f; }
        if (GetKeyState('D') & 0x8000) { camera_pos.X() += 0.2f; }

        Camera camera = {};
        camera.SetPosition(camera_pos);

        m_instance_allocator->Reset(ctx.current_frame_index);

        instances.clear();

        auto view = registry.view<MeshComponent, TransformComponent>();

        for (auto [ent, mesh, transform] : view.each()) 
        {
            Renderer::InstanceData instance = {};
            instance.world_matrix = transform.GetLocalToWorld();
            instance.mesh_offset = RenderResourceManager::Get().GetGPUMeshOffset(mesh.mesh_index);
            instance.material_table_offset = RenderResourceManager::Get().GetGPUMaterialTableOffset(mesh.material_table_index);

            instances.push_back(instance);
        }

        m_instance_allocator->WriteData(instances.data(), instances.size());

        Constants constants = {};
        constants.view_proj = camera.GetViewProjectionMatrix();
        constants.instance_num = instances.size();

        {
            auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<BufferIndices>(&indices);

            list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 0);
        }

        {
            auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<Constants>(&constants);

            list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);
        }

        list->SetGraphicsSamplerState(RenderCore::Get().GetPresetSampler(ESamplerPresetType::LinearClamp), 2);

        native_list->DispatchMesh(instances.size(), 1, 1);

		GUISystem::Get().BeginFrame();

        GUISystem::Get().EndFrame();

        GUISystem::Get().Flush(ctx);


        RenderCore::Get().EndFrame();
    }

    AssetManager::Shutdown();

    RenderCore::Shutdown();

    WindowSystem::Shutdown();

    system("pause");
    return 0;
}

