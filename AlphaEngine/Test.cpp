#include <iostream>
#include <d3d12.h>
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
#include "Source/Core/Window/WindowSystem.h"
#include "Source/Runtime/RenderCore/RenderCore.h"
#include "Source/Runtime/RenderCore/RenderHardwareInterface.h"
#include "Source/Runtime/RenderCore/Module.h"

#include "Source/Runtime/Renderer/GUISystem.h"
#include "Source/Runtime/Renderer/RenderContext.h"
#include "Source/Runtime/Renderer/Definitions.h"
#include "Source/Runtime/Renderer/RenderResourceManager.h"

#include "Source/Runtime/Asset/Module.h"
#include "Source/Runtime/Animation/Module.h"
#include "Source/Runtime/Renderer/Camera.h"

#include "Source/Core/Serialization/ByteWriter.h"

#include "ThirdParty/meshoptimizer/meshoptimizer.h"

#include "ThirdParty/imgui/imgui.h"

#include "Source/Core/Math/MathF.h"

#include "RHI/Definitions.h"

#pragma comment(lib, "dxguid.lib")

using Microsoft::WRL::ComPtr;
using namespace Alpha;
using namespace Alpha::Renderer;

// Work Graphsを管理するオブジェクト一式
ComPtr<ID3D12StateObject>       g_pWorkGraphStateObject;
ComPtr<ID3D12WorkGraphProperties> g_pWorkGraphProps;
ComPtr<ID3D12Resource>          g_pBackingMemory;

void InitializeWorkGraph(ID3D12Device9* device, const void* pShaderBytecode, SIZE_T bytecodeLength)
{

    // サブオブジェクト（設定項目）の配列を用意
    std::vector<D3D12_STATE_SUBOBJECT> subobjects;

    // ① DXILライブラリ（コンパイル済みのHLSLバイナリ）を登録する設定
    D3D12_DXIL_LIBRARY_DESC dxilDesc = {};
    dxilDesc.DXILLibrary.pShaderBytecode = pShaderBytecode;
    dxilDesc.DXILLibrary.BytecodeLength = bytecodeLength;

    D3D12_STATE_SUBOBJECT dxilSubobject = {};
    dxilSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
    dxilSubobject.pDesc = &dxilDesc;
    subobjects.push_back(dxilSubobject);

    // ② 「Work Graphs」としてビルドすることを宣言する設定
    D3D12_WORK_GRAPH_DESC wgDesc = {};
    wgDesc.ProgramName = L"MyWorkGraph"; // グラフの名前（任意）
    wgDesc.Flags = D3D12_WORK_GRAPH_FLAG_NONE;

    D3D12_STATE_SUBOBJECT wgSubobject = {};
    wgSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_WORK_GRAPH;
    wgSubobject.pDesc = &wgDesc;
    
    subobjects.push_back(wgSubobject);

    D3D12_GENERIC_PROGRAM_DESC drawingProgram = {};
    drawingProgram.ProgramName = L"MyRenderNode"; // この名前がWork Graphのノード名になる

    // 結合したい「MS」と「PS」の関数名を配列で指定
    LPCWSTR programExports[] = { L"RenderNode", L"MyPixelShader" }; //
    drawingProgram.pExports = programExports;
    drawingProgram.NumExports = 2;

    D3D12_STATE_SUBOBJECT programSubobject = {};
    programSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_GENERIC_PROGRAM; // ★プログラムとして合体！
    programSubobject.pDesc = &drawingProgram;
    subobjects.push_back(programSubobject);


    // 最上位の StateObject 記述子を組み立てる
    D3D12_STATE_OBJECT_DESC stateObjectDesc = {};
    stateObjectDesc.Type = D3D12_STATE_OBJECT_TYPE_COLLECTION; // グラフをまとめる形式
    stateObjectDesc.NumSubobjects = static_cast<UINT>(subobjects.size());
    stateObjectDesc.pSubobjects = subobjects.data();

    // GPUドライバーにコンパイルさせてグラフ（State Object）を生成
    device->CreateStateObject(&stateObjectDesc, IID_PPV_ARGS(&g_pWorkGraphStateObject));

    // =========================================================================
    // 2. メモリ見積もりとバッキングメモリの確保
    // =========================================================================

    // 作成したStateObjectから、Work Graphs専用のインターフェースを取り出す
    g_pWorkGraphStateObject.As(&g_pWorkGraphProps);

    // グラフのインデックスを取得
    UINT programIndex = g_pWorkGraphProps->GetWorkGraphIndex(L"MyWorkGraph");

    // GPUが内部でレコードをバケツリレーするのに必要な「作業用メモリ（バッキングメモリ）」のサイズを見積もる
    D3D12_WORK_GRAPH_MEMORY_REQUIREMENTS memReqs = {};
    g_pWorkGraphProps->GetWorkGraphMemoryRequirements(programIndex, &memReqs);

    // 見積もられたサイズでVRAM上のバッファ（UAVフラグ付き）を確保する
    D3D12_HEAP_PROPERTIES heapProps = { D3D12_HEAP_TYPE_DEFAULT };
    D3D12_RESOURCE_DESC resDesc = {};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = memReqs.MaxSizeInBytes; // 要求された最大サイズ
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.SampleDesc.Count = 1;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS; // UAVが必要

    device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&g_pBackingMemory));
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
    auto model3 = AssetManager::Get().GetCube();
    
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
    };
    RenderResourceManager::Startup(resource_args);

    UInt32 akari_index = RenderResourceManager::Get().RegisterMesh(model1);
    UInt32 patchouli_index = RenderResourceManager::Get().RegisterMesh(model2);
    UInt32 sphere_index = RenderResourceManager::Get().RegisterMesh(model3);


    RenderCore::Get().PostInitialize();

    auto ms = RenderCore::Get().CreateShader(L"Asset/TestMeshShader.hlsl", ERHIShaderType::Mesh);
    auto ps = RenderCore::Get().CreateShader(L"Asset/PixelShader.hlsl", ERHIShaderType::Pixel);

    auto layout = RenderCore::Get().CreatePipelineLayout(
        [](PipelineLayout& layout) 
        {
            layout.AddRootCBV(1, ERHIShaderFlags::Mesh);
            layout.AddSampler(1, ERHIShaderFlags::Pixel);
			layout.AddRootCBV(1, ERHIShaderFlags::Pixel);
            layout.SetFlags(ERHIPipelineLayoutFlags::Bindless);
        }
    );

    auto pipeline = RenderCore::Get().CreateGraphicsPipeline(
        {
            .pipeline_layout = layout.get(),
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

    struct Constants
    {
        float4x4 ViewProj;
        RenderResourceManager::HeapIndices indices{};
        UInt32 instance_heap{};
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

        Constants constants = {};
        constants.ViewProj = camera.GetViewProjectionMatrix();
        constants.indices = RenderResourceManager::Get().GetBufferHeapIndices();
        constants.instance_heap = m_instance_allocator->GetBuffer()->GetSRVHandle().index;

        {
            auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<Constants>(&constants);

            list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 0);
            list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);
        }
        list->SetGraphicsSamplerState(RenderCore::Get().GetPresetSampler(ESamplerPresetType::LinearClamp), 1);
	
        const auto& mesh1 = RenderResourceManager::Get().GetMesh(akari_index);
        const float interval = 20.0f;

        instances.clear();

        for (int i = -5; i < 5; ++i)
        {
            for (int j = -5; j < 5; ++j)
            {
                float posX = interval * j;
                float posZ = 0.0f;
                float posY = interval * i;

                Matrix4x4 pos = Matrix4x4::Identity;
                pos._4() = float3(posX, posY, posZ);
   
                const auto& table = RenderResourceManager::Get().GetMaterialTable(mesh1.default_table_offset);


                Renderer::InstanceData instance = {};
                instance.world_matrix = pos;
                instance.mesh_index = akari_index;
                instance.material_table_index = table.alloc.offset;

                instances.push_back(instance);
            }
        }

        m_instance_allocator->WriteData(instances.data(), instances.size());

        native_list->DispatchMesh(mesh1.meshlets.size(), instances.size(), 1);

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


/*
* Workgraphテストコード
* 
	auto shader = RenderCore::Get().CreateShader(L"Asset/TestWork.hlsl", ERHIShaderType::Library);
    auto s = static_cast<D3D12Shader*>(shader->GetRHI());

	auto layout = RenderCore::Get().CreatePipelineLayout(
		[](PipelineLayout& layout)
		{
            layout.SetFlags(ERHIPipelineLayoutFlags::Bindless);
		}
	);

    auto l = static_cast<D3D12PipelineLayout*>(layout->GetRHI());

    auto d = static_cast<D3D12Dynamic*>(RenderCore::Get().GetRHI()->GetDynamic());

    auto device = d->GetDevice()->GetDevice();
    // 1. DXILライブラリの設定（コンパイル済みシェーダーを登録）
    D3D12_DXIL_LIBRARY_DESC dxilLibDesc = {};
    dxilLibDesc.DXILLibrary = { s->GetBytecodePointer(), s->GetBytecodeLength() }; // コンパイルしたデータ
    dxilLibDesc.NumExports = 0; // 0にするとライブラリ内の全エクスポートが対象

    D3D12_STATE_SUBOBJECT subobjects[3] = {};

    // --- Subobject 0: DXIL Library ---
    subobjects[0].Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
    subobjects[0].pDesc = &dxilLibDesc;

    // --- Subobject 1: Work Graph ---
    D3D12_WORK_GRAPH_DESC workGraphDesc = {};
    workGraphDesc.ProgramName = L"MyParentNode";
    workGraphDesc.Flags = D3D12_WORK_GRAPH_FLAG_INCLUDE_ALL_AVAILABLE_NODES;

    subobjects[1].Type = D3D12_STATE_SUBOBJECT_TYPE_WORK_GRAPH;
    subobjects[1].pDesc = &workGraphDesc;

    auto root = l->GetNativeHandle();
    subobjects[2].Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
	subobjects[2].pDesc = &root;

    D3D12_STATE_OBJECT_DESC stateObjectDesc = {};
    stateObjectDesc.Type = D3D12_STATE_OBJECT_TYPE_EXECUTABLE; // Work Graphsはこれ
    stateObjectDesc.NumSubobjects = 3;
    stateObjectDesc.pSubobjects = subobjects;
    D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION;
    D3D12_EXPORT_DESC;
    ComPtr<ID3D12StateObject> pStateObject;
    HRESULT hr = device->CreateStateObject(&stateObjectDesc, IID_PPV_ARGS(&pStateObject));

    if (FAILED(hr)) {
        return hr;
    }
	ID3D12StateObjectProperties1* pStateObjectProps = nullptr;
    pStateObject->QueryInterface(IID_PPV_ARGS(&pStateObjectProps));

    ID3D12WorkGraphProperties1* pWorkGraphProps = nullptr;
    pStateObject->QueryInterface(IID_PPV_ARGS(&pWorkGraphProps));

    UINT workGraphIndex = 0;
    D3D12_WORK_GRAPH_MEMORY_REQUIREMENTS memReqs = {};
    pWorkGraphProps->GetWorkGraphMemoryRequirements(workGraphIndex, &memReqs);


    std::cout << "Backing Memory Required: " << memReqs.MaxSizeInBytes << " bytes" << std::endl;

    auto backing = RenderCore::Get().CreateBuffer(1, memReqs.MaxSizeInBytes, EBufferPresetType::RWStructured);

    auto result_write = RenderCore::Get().CreateBuffer(sizeof(uint32_t), sizeof(uint32_t), EBufferPresetType::RWStructured);

    auto result = RenderCore::Get().CreateBuffer(sizeof(uint32_t), sizeof(uint32_t), EBufferPresetType::Readback);
    auto handle = RenderCore::Get().GetBindlessHeap()->Allocate();

    RHIResourceViewDesc dsv_desc = {};
    dsv_desc.resource = result_write->GetRHI();
    dsv_desc.heap_index = handle.index;
    dsv_desc.buffer_stride = result_write->GetStride();
    dsv_desc.type = ERHIResourceViewType::UnorderedAccessView;

    handle.heap->GetRHI()->CreateView(dsv_desc);

    auto list = RenderCore::Get().GetCommandList();
    list->Begin(0);
    list->SetDescriptorHeap(RenderCore::Get().GetBindlessHeap(), RenderCore::Get().GetSamplerHeap());

    auto lis = static_cast<D3D12CommandBuffer*>(list->GetRHI());
    
    D3D12_SET_PROGRAM_DESC setProgram = {};
    setProgram.Type = D3D12_PROGRAM_TYPE_WORK_GRAPH;
    setProgram.WorkGraph.ProgramIdentifier = pStateObjectProps->GetProgramIdentifier(L"MyParentNode");
    setProgram.WorkGraph.BackingMemory = { backing->GetRHI()->GetGPUAddress(), backing->GetSize()};
    setProgram.WorkGraph.Flags = D3D12_SET_WORK_GRAPH_FLAG_INITIALIZE;

	lis->GetNative()->SetComputeRootSignature(l->GetNativeHandle());
    lis->GetNative()->SetProgram(&setProgram);

    struct MyData
    {
        uint32_t value;
	};
    MyData inputData = { 42 }; // 初期値

    D3D12_NODE_ID entryNodeID = { L"MyParentNode", 0 };
    UINT entryIndex = pWorkGraphProps->GetEntrypointIndex(0, entryNodeID);

    D3D12_DISPATCH_GRAPH_DESC dispatchDesc = {};
    dispatchDesc.Mode = D3D12_DISPATCH_MODE_NODE_CPU_INPUT;
    dispatchDesc.NodeCPUInput.EntrypointIndex = entryIndex;
    dispatchDesc.NodeCPUInput.NumRecords = 1;
    dispatchDesc.NodeCPUInput.pRecords = &inputData;
    dispatchDesc.NodeCPUInput.RecordStrideInBytes = sizeof(MyData);
    list->Transition(backing.get(), ERHIResourceState::UnorderedAccess);
    lis->GetNative()->DispatchGraph(&dispatchDesc);

	list->UAVBarrier(result_write.get());
    list->Transition(result_write.get(), ERHIResourceState::CopySource);
    list->Transition(result.get(), ERHIResourceState::CopyDestination);
    list->CopyBuffer(result_write.get(), result.get());

    list->End();

    RenderCore::Get().GetCommandQueue()->Execute(list);
	RenderCore::Get().GetCommandQueue()->Signal(RenderCore::Get().GetTimeline());

	RenderCore::Get().GetTimeline()->WaitCPU();

    auto data = result->GetRHI()->Map(0, sizeof(MyData));

    uint32_t* pUintData = reinterpret_cast<uint32_t*>(data);
    std::cout << "Result from Work Graph: " << *pUintData << std::endl;


*/