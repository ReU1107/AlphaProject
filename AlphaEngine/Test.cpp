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

namespace
{
    static DXGI_FORMAT GetDXGIFormat(Alpha::ERHIResourceFormat format)
    {
        switch (format)
        {
        case Alpha::ERHIResourceFormat::RGBAUnorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Alpha::ERHIResourceFormat::Depth24Stencil8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case Alpha::ERHIResourceFormat::Depth16: return DXGI_FORMAT_D16_UNORM;
        case Alpha::ERHIResourceFormat::Depth32: return DXGI_FORMAT_D32_FLOAT;
        case Alpha::ERHIResourceFormat::RGBFloat: return DXGI_FORMAT_R32G32B32_FLOAT;
        case Alpha::ERHIResourceFormat::RGBAFloat: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Alpha::ERHIResourceFormat::RGFloat: return DXGI_FORMAT_R32G32_FLOAT;
        case Alpha::ERHIResourceFormat::RGBA32UInt: return DXGI_FORMAT_R32G32B32A32_UINT;

        default:
            return DXGI_FORMAT_UNKNOWN;
        }

        return DXGI_FORMAT_UNKNOWN;
    }

    static D3D12_PRIMITIVE_TOPOLOGY_TYPE GetD3D12PrimitiveTopologyType(Alpha::ERHIPrimitiveTopology topology)
    {
        switch (topology)
        {
        case Alpha::ERHIPrimitiveTopology::TriangleList:	return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        case Alpha::ERHIPrimitiveTopology::LineList:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        case Alpha::ERHIPrimitiveTopology::PointList:		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
        default:
            return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
        }

        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
    }

    static D3D12_FILL_MODE GetD3D12FillMode(Alpha::ERHIFillMode fill_mode)
    {
        switch (fill_mode)
        {
        case Alpha::ERHIFillMode::Solid:		return D3D12_FILL_MODE_SOLID;
        case Alpha::ERHIFillMode::Wireframe:	return D3D12_FILL_MODE_WIREFRAME;
        default:
            return D3D12_FILL_MODE_SOLID;
        }
        return D3D12_FILL_MODE_SOLID;
    }

    static D3D12_CULL_MODE GetD3D12CullMode(Alpha::ERHICullMode cull_mode)
    {
        switch (cull_mode)
        {
        case Alpha::ERHICullMode::None:		return D3D12_CULL_MODE_NONE;
        case Alpha::ERHICullMode::Front:	return D3D12_CULL_MODE_FRONT;
        case Alpha::ERHICullMode::Back:		return D3D12_CULL_MODE_BACK;
        default:
            return D3D12_CULL_MODE_BACK;
        }
        return D3D12_CULL_MODE_BACK;
    }

    static D3D12_BLEND GetD3D12Blend(Alpha::ERHIBlend blend_mode)
    {
        switch (blend_mode)
        {
        case Alpha::ERHIBlend::Zero: return D3D12_BLEND_ZERO;
        case Alpha::ERHIBlend::One: return D3D12_BLEND_ONE;
        case Alpha::ERHIBlend::SrcColor: return D3D12_BLEND_SRC_COLOR;
        case Alpha::ERHIBlend::InvSrcColor: return D3D12_BLEND_INV_SRC_COLOR;
        case Alpha::ERHIBlend::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
        case Alpha::ERHIBlend::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
        case Alpha::ERHIBlend::DestAlpha: return D3D12_BLEND_DEST_ALPHA;
        case Alpha::ERHIBlend::InvDestAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
        case Alpha::ERHIBlend::DestColor: return D3D12_BLEND_DEST_COLOR;
        case Alpha::ERHIBlend::InvDestColor: return D3D12_BLEND_INV_DEST_COLOR;
        case Alpha::ERHIBlend::SrcAlphaSat: return D3D12_BLEND_SRC_ALPHA_SAT;
        case Alpha::ERHIBlend::BlendFactor: return D3D12_BLEND_BLEND_FACTOR;
        case Alpha::ERHIBlend::InvBlendFactor: return D3D12_BLEND_INV_BLEND_FACTOR;
        default:
            break;
        }
        return D3D12_BLEND_ZERO;;
    }

    static D3D12_BLEND_OP GetD3D12BlendOp(Alpha::ERHIBlendOp blend_op)
    {
        switch (blend_op)
        {
        case Alpha::ERHIBlendOp::Add: return D3D12_BLEND_OP_ADD;
        case Alpha::ERHIBlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
        case Alpha::ERHIBlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
        case Alpha::ERHIBlendOp::Min: return D3D12_BLEND_OP_MIN;
        case Alpha::ERHIBlendOp::Max: return D3D12_BLEND_OP_MAX;
        default:
            break;
        }
        return D3D12_BLEND_OP_ADD;
    }

    static D3D12_RENDER_TARGET_BLEND_DESC GetBlendDesc(Alpha::RHIBlendStateDesc d)
    {
        D3D12_RENDER_TARGET_BLEND_DESC desc =
        {
            .BlendEnable = d.blend_enable,
            .LogicOpEnable = d.logic_op_enable,
            .SrcBlend = GetD3D12Blend(d.src_blend),
            .DestBlend = GetD3D12Blend(d.dest_blend),
            .BlendOp = GetD3D12BlendOp(d.blend_op),
            .SrcBlendAlpha = GetD3D12Blend(d.src_blend_alpha),
            .DestBlendAlpha = GetD3D12Blend(d.dest_blend_alpha),
            .BlendOpAlpha = GetD3D12BlendOp(d.blend_op_alpha),
            .LogicOp = D3D12_LOGIC_OP_NOOP,
            .RenderTargetWriteMask = d.write_mask
        };
        return desc;
    }

    static D3D12_RASTERIZER_DESC GetDefaultRasterizerDesc()
    {
        D3D12_RASTERIZER_DESC desc = {};
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.CullMode = D3D12_CULL_MODE_BACK;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        desc.DepthClipEnable = TRUE;
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
        desc.ForcedSampleCount = 0;
        desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        return desc;
    }

}

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

/*
* 将来的にはmeshshaderとかpixelshaderのくくりを外して汎用的にする
*/
struct RHIWorkGraphProgramDesc
{
    String name{ L"Draw" };
    RHIShader* mesh_shader{ nullptr };
    RHIShader* pixel_shader{ nullptr };
    /*
    * boolとかで使用するステート情報を加算していくイメージ
    */
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

    D3D12_FEATURE_DATA_D3D12_OPTIONS21 Options = {};
    device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS21, &Options, sizeof(Options));

    if (Options.WorkGraphsTier < D3D12_WORK_GRAPHS_TIER_1_1)
    {
        ALPHA_LOG_ERROR(L"meshshadernodeが対応していません;;");
    }


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