#pragma once

namespace Alpha
{

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

/*
*
* WorkGraphラッピング用
struct RHIGraphicsStateDesc
{
    RHIRasterizerStateDesc rasterizer{};
    RHIBlendStateDesc blend_states[8]{};
    ERHIResourceFormat render_targets[8]{};
    UInt32 render_target_count{ 0 };
    ERHIResourceFormat depth_format{ ERHIResourceFormat::Depth24Stencil8 };

    ERHIPrimitiveTopology primitive_topology{ ERHIPrimitiveTopology::TriangleList };


    ERHIFillMode fill_mode{ ERHIFillMode::Solid };
    ERHICullMode cull_mode{ ERHICullMode::Back };

    Bool alpha_to_coverage_enable{ false };
    Bool independent_blend_enable{ false };
    Bool depth_enable{ false };
};

// 将来的にはmeshshaderとかpixelshaderのくくりを外して汎用的にする
struct RHIWorkGraphProgramDesc
{
    String name{ L"Draw" };
    RHIShader* mesh_shader{ nullptr };
    RHIShader* pixel_shader{ nullptr };
// boolとかで使用するステート情報を加算していくイメージ
};

struct RHIDynamicPipelineDesc
{
    String graph_name{ L"WorkGraph" };

    RHIPipelineLayout* layout{ nullptr };

    TArray<RHIShader*> libraries{};     //

    // 将来的に配列にして汎用性を..
    RHIWorkGraphProgramDesc program{};

    RHIGraphicsStateDesc graphics_state{};

    bool is_graphics{ false };

};

void InitializeWorkGraph(ID3D12Device9* device, RHIDynamicPipelineDesc& desc)
{


    CD3DX12_STATE_OBJECT_DESC state_object_desc(D3D12_STATE_OBJECT_TYPE_EXECUTABLE);


    auto pipeline_layout = static_cast<D3D12PipelineLayout*>(desc.layout);
    auto root_signature = state_object_desc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
    root_signature->SetRootSignature(pipeline_layout->GetNativeHandle());

    // このワークグラフで使用するノードが入ったシェーダーを登録していく
    // 1hlslファイルに対して:1ノードになるようにする
    for (auto shader : desc.libraries)
    {
        auto d3d12_shader = static_cast<D3D12Shader*>(shader);

        auto shader_bytecode = CD3DX12_SHADER_BYTECODE(d3d12_shader->GetBytecodePointer(), d3d12_shader->GetBytecodeLength());

        auto library_subobject = state_object_desc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
        library_subobject->SetDXILLibrary(&shader_bytecode);
    }

    if (desc.is_graphics)
    {
        // 従来のラスタライズ機能を使うっていうやつ

        auto config_subobject = state_object_desc.CreateSubobject<CD3DX12_STATE_OBJECT_CONFIG_SUBOBJECT>();
        config_subobject->SetFlags(D3D12_STATE_OBJECT_FLAG_WORK_GRAPHS_USE_GRAPHICS_STATE_FOR_GLOBAL_ROOT_SIGNATURE);


        auto rasterizer_subobject = state_object_desc.CreateSubobject<CD3DX12_RASTERIZER_SUBOBJECT>();


        auto primitive_topology_subobject = state_object_desc.CreateSubobject<CD3DX12_PRIMITIVE_TOPOLOGY_SUBOBJECT>();
        primitive_topology_subobject->SetPrimitiveTopologyType(GetD3D12PrimitiveTopologyType(desc.graphics_state.primitive_topology));


        auto depth_stencil_subobject = state_object_desc.CreateSubobject<CD3DX12_DEPTH_STENCIL_FORMAT_SUBOBJECT>();
        depth_stencil_subobject->SetDepthStencilFormat(GetDXGIFormat(desc.graphics_state.depth_format));


        auto render_target_subobject = state_object_desc.CreateSubobject<CD3DX12_RENDER_TARGET_FORMATS_SUBOBJECT>();
        render_target_subobject->SetNumRenderTargets(desc.graphics_state.render_target_count);
        auto blend_subobject = state_object_desc.CreateSubobject<CD3DX12_BLEND_SUBOBJECT>();

        for (UInt32 i = 0; i < desc.graphics_state.render_target_count; i++)
        {
            render_target_subobject->SetRenderTargetFormat(i, GetDXGIFormat(desc.graphics_state.render_targets[i]));
            blend_subobject->SetRenderTarget(i, GetBlendDesc(desc.graphics_state.blend_states[i]));
        }


        auto sample_desc_subobject = state_object_desc.CreateSubobject<CD3DX12_SAMPLE_DESC_SUBOBJECT>();
        sample_desc_subobject->SetCount(1);
        sample_desc_subobject->SetQuality(0);

        auto generic_program = state_object_desc.CreateSubobject<CD3DX12_GENERIC_PROGRAM_SUBOBJECT>();
        generic_program->SetProgramName(desc.program.name.c_str());

        auto ms_shader = static_cast<D3D12Shader*>(desc.program.mesh_shader);

        auto ps_shader = static_cast<D3D12Shader*>(desc.program.pixel_shader);


        generic_program->AddExport(ms_shader->GetEntryPointName());
        generic_program->AddExport(ps_shader->GetEntryPointName());

        // このグラフィックスノード（GenericProgram）に適用するステートのポインタを紐付け
        generic_program->AddSubobject(*rasterizer_subobject);
        generic_program->AddSubobject(*primitive_topology_subobject);
        generic_program->AddSubobject(*depth_stencil_subobject);
        generic_program->AddSubobject(*render_target_subobject);
        generic_program->AddSubobject(*sample_desc_subobject);


    }

    auto work_graph_subobject = state_object_desc.CreateSubobject<CD3DX12_WORK_GRAPH_SUBOBJECT>();
    work_graph_subobject->SetProgramName(L"MyWorkGraphProgram");
    work_graph_subobject->IncludeAllAvailableNodes();


    HRESULT hr = device->CreateStateObject(state_object_desc, IID_PPV_ARGS(&g_pWorkGraphStateObject));
    if (FAILED(hr))
    {
        __debugbreak();
    }

    // 作成したStateObjectから、Work Graphs専用のインターフェースを取り出す
    g_pWorkGraphStateObject.As(&g_pWorkGraphProps);

    // グラフのインデックスを取得
    UINT programIndex = g_pWorkGraphProps->GetWorkGraphIndex(L"MyWorkGraph");

    // GPUが内部でレコードをバケツリレーするのに必要な「作業用メモリ（バッキングメモリ）」のサイズを見積もる
    D3D12_WORK_GRAPH_MEMORY_REQUIREMENTS memReqs = {};
    g_pWorkGraphProps->GetWorkGraphMemoryRequirements(programIndex, &memReqs);

    ComPtr<ID3D12StateObjectProperties1> stateObjectProperties;
    ComPtr<ID3D12WorkGraphProperties>    workGraphProperties;

    g_pWorkGraphStateObject->QueryInterface(IID_PPV_ARGS(&stateObjectProperties));
    g_pWorkGraphStateObject->QueryInterface(IID_PPV_ARGS(&workGraphProperties));

    ComPtr<ID3D12WorkGraphProperties1> workGraphProperties1;
    //stateObject_->QueryInterface(IID_PPV_ARGS(&workGraphProperties1));

    //workGraphProperties1->SetMaximumInputRecords(workGraphIndex, 1, 1);



    auto backing = RenderCore::Get().CreateBuffer(1, memReqs.MaxSizeInBytes, EBufferPresetType::RWStructured);


}
*/