#include <iostream>
#include <d3d12.h>
#include <wrl.h>
#include <d3d12sdklayers.h>

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

#include "RHI/Definitions.h"

using Microsoft::WRL::ComPtr;
using namespace Alpha;
int main() 
{
    WindowSystem::Args win_args =
    {
        .title = L"Graphics Engine",
        .width = 1920,
        .height = 1080,
        .fullscreen = false
    };

    if (!WindowSystem::Startup(win_args))
        return -1;

    RenderCore::Args render_core_args = {};
    render_core_args.hWnd = (HWND)WindowSystem::Get().GetNativeHandle();
    render_core_args.transient_buffer_size = 1_MB;
    RenderCore::Startup(render_core_args);

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

    auto backing = RenderCore::Get().CreateStructuredBuffer(1, memReqs.MaxSizeInBytes, EResourceAccessFlags::Write);

    auto result_write = RenderCore::Get().CreateStructuredBuffer(sizeof(uint32_t), 1, EResourceAccessFlags::Write);

    auto result = RenderCore::Get().CreateStructuredBuffer(sizeof(uint32_t), 1, EResourceAccessFlags::CPURead);
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


    result->GetRHI()->Unmap(0, sizeof(MyData));

    system("pause");
    return 0;
}
