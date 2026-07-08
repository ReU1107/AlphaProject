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

#include "Source/Runtime/Asset/Module.h"
#include "Source/Runtime/Animation/Module.h"
#include "Source/Runtime/Renderer/Camera.h"

#include "Source/Core/Serialization/ByteWriter.h"

#include "ThirdParty/meshoptimizer/meshoptimizer.h"

#include "ThirdParty/imgui/imgui.h"

#include "RHI/Definitions.h"

#pragma comment(lib, "dxguid.lib")


using Microsoft::WRL::ComPtr;
using namespace Alpha;

struct Meshlet
{
    UInt32 vertex_offset{};
    UInt32 triangle_offset{};

    UInt32 vertex_count{};
    UInt32 triangle_count{};
    UInt32 material_index{};
};

struct GPUMaterialData 
{
    Renderer::MaterialData data{};
    UInt32 base_color_tex_idx{ 0xFFFFFFFF };
    UInt32 normal_tex_idx{ 0xFFFFFFFF };
    UInt32 metallic_roughness_tex_idx{ 0xFFFFFFFF };
    UInt32 emissive_tex_idx{ 0xFFFFFFFF };

    UInt32 occlusion_tex_idx{ 0xFFFFFFFF };
    UInt32 toon_tex_idx{ 0xFFFFFFFF };
    UInt32 sphere_tex_idx{ 0xFFFFFFFF };
    UInt32 flags{ 0 }; // マテリアルの種類（不透明・半透明など）を判別するフラグ用
};


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
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

    AssetManager::Args asset_args =
    {

    };
    AssetManager::Startup(asset_args);
    auto model = AssetManager::Get().Load<ModelHandle>(L"C:/Users/owner/Desktop/Asset/紲星☆あかり20180430/お宮式紲星☆あかりv1.00.pmx");

    RenderCore::Args render_core_args = {};
    render_core_args.hWnd = (HWND)WindowSystem::Get().GetNativeHandle();
    render_core_args.transient_buffer_size = 1_MB;
    RenderCore::Startup(render_core_args);

    GUISystem::Args gui_args = {
        .render_core = RenderCore::GetPtr(),
    };
    GUISystem::Startup(gui_args);

    auto vertex_buffer = std::make_unique<PersistentBufferAllocator>();
    {
        vertex_buffer->Initialize
        (
            {
                .render_core = RenderCore::GetPtr(),
                .alignment = sizeof(Vector3),
                .max_size = 12_MB,
                .preset = EBufferPresetType::ROStructured,
            });
    }
    vertex_buffer->GetBuffer()->SetName(L"VertexPositionBuffer");
    auto meshlet_buffer = std::make_unique<PersistentBufferAllocator>();
    {
        meshlet_buffer->Initialize(
            {
                .render_core = RenderCore::GetPtr(),
                .alignment = sizeof(Meshlet),
                .max_size = 12_MB,
                .preset = EBufferPresetType::ROStructured,
            });
    }
    meshlet_buffer->GetBuffer()->SetName(L"MeshletBuffer");

    auto vertex_indices_buffer = std::make_unique<PersistentBufferAllocator>();
    {
        vertex_indices_buffer->Initialize(
            {
                .render_core = RenderCore::GetPtr(),
                .alignment = sizeof(UInt32),
                .max_size = 12_MB,
                .preset = EBufferPresetType::ROStructured,
            });
        vertex_indices_buffer->GetBuffer()->SetName(L"VertexIndicesBuffer");
    }

    auto triangle_indices_buffer = std::make_unique<PersistentBufferAllocator>();
    {
        triangle_indices_buffer->Initialize(
            {
                .render_core = RenderCore::GetPtr(),
                .alignment = sizeof(UInt32),
                .max_size = 12_MB,
                .preset = EBufferPresetType::ROStructured,
            });
        triangle_indices_buffer->GetBuffer()->SetName(L"TriangleIndicesBuffer");
    }

    // position以外の頂点データを格納
    auto vertex_attribute_buffer = std::make_unique<PersistentBufferAllocator>();
    {
        vertex_attribute_buffer->Initialize(
            {
                .render_core = RenderCore::GetPtr(),
                .alignment = 4,
                .max_size = 12_MB,
                .preset = EBufferPresetType::ROByteAddress,
            });
        vertex_attribute_buffer->GetBuffer()->SetName(L"VertexAttributeBuffer");

    }

    auto material_buffer = std::make_unique<PersistentBufferAllocator>();
    {
        material_buffer->Initialize(
            {
                .render_core = RenderCore::GetPtr(),
                .alignment = sizeof(GPUMaterialData),
                .max_size = 4_MB,
                .preset = EBufferPresetType::ROStructured,
            });
        material_buffer->GetBuffer()->SetName(L"MaterialBuffer");
    }

    UInt64 global_vertex_offset = 0;
    {
        UInt64 offset = 0;
        UInt32 num = model->GetPositions().size();
        vertex_buffer->Allocate(num, &offset);
        RenderCore::Get().UploadBuffer(vertex_buffer->GetBuffer(), offset, model->GetPositions().data(), sizeof(Vector3) * num);

        global_vertex_offset = offset / sizeof(Vector3);
    }

    UInt64 global_vertex_attribute_offset = 0;
    {
        UInt64 offset = 0;
        UInt32 size = model->GetPackData().size();

        vertex_attribute_buffer->AllocateBySize(size, &offset);
        RenderCore::Get().UploadBuffer(vertex_attribute_buffer->GetBuffer(), offset, model->GetPackData().data(), size);

        global_vertex_attribute_offset = offset;
    }

    TMap<UInt64, TShared<Texture>> m_texture_cache{};// 今後全モデルのやつになるから

    auto CreateTexture = [&](const TShared<TextureHandle>& texture_handle) -> UInt32 {
        if (!texture_handle) return 0xFFFFFFFF;

        auto texture = RenderCore::Get().CreateTexture(
            texture_handle->GetWidth(),
            texture_handle->GetHeight(),
            texture_handle->GetBindFlags(),
            texture_handle->GetFormat(),
            texture_handle->GetDimension()
        );
        RenderCore::Get().UploadTexture(texture.get(), texture_handle->GetPixels());

        UInt32 descriptor_index = texture->GetSRVHandle().index;
        m_texture_cache[texture_handle->GetID()] = std::move(texture);
        return descriptor_index;
    };

    // 先にビルドインテクスチャの作成
    auto white_tex = CreateTexture(AssetManager::Get().GetWhiteTexture());
    auto black_tex = CreateTexture(AssetManager::Get().GetBlackTexture());
    auto normal_tex = CreateTexture(AssetManager::Get().GetNormalTexture());


    TArray<GPUMaterialData> materials;

    UInt64 global_material_index = 0;
    {
        for (auto& material_handle : model->GetMaterialHandles())
        {
            // このマテリアルが利用するテクスチャのsrvindex
            TMap<ETextureAttribute, UInt32> local_tex_cache;

            for (auto& texture_handle : material_handle->GetTextureHandles())
            {
                ETextureAttribute attr = texture_handle->GetAttribute();
                if (attr == ETextureAttribute::None) continue;

                UInt32 descriptor_index = 0xFFFFFFFF;
                auto found = m_texture_cache.find(texture_handle->GetID());
                if (found != m_texture_cache.cend())
                {
                    descriptor_index = found->second->GetSRVHandle().index;
                }
                else
                {
                    descriptor_index = CreateTexture(texture_handle);
                }

                local_tex_cache[attr] = descriptor_index;
            }
            auto GetTexture = [&](ETextureAttribute attr, UInt32 fallback_srv)
            {
                auto found = local_tex_cache.find(attr);
                return (found != local_tex_cache.cend()) ? found->second : fallback_srv;
            };


            GPUMaterialData material = {};
            material.data = material_handle->GetData();

            material.base_color_tex_idx = GetTexture(ETextureAttribute::BaseColor, white_tex);
            material.normal_tex_idx = GetTexture(ETextureAttribute::Normal, normal_tex);
            material.metallic_roughness_tex_idx = GetTexture(ETextureAttribute::Metallic, white_tex);
            material.emissive_tex_idx = GetTexture(ETextureAttribute::Emissive, black_tex);
            material.occlusion_tex_idx = GetTexture(ETextureAttribute::Occlusion, white_tex);
            material.toon_tex_idx = GetTexture(ETextureAttribute::Toon, white_tex);
            material.sphere_tex_idx = GetTexture(ETextureAttribute::Sphere, white_tex);


            materials.push_back(material);

        }

        UInt64 offset = 0;
        UInt32 num = materials.size();

        material_buffer->Allocate(num, &offset);

        RenderCore::Get().UploadBuffer(material_buffer->GetBuffer(), offset, materials.data(), num * sizeof(GPUMaterialData));

        // offsetがバイト単位なのでindexに変換
        global_material_index = offset / sizeof(GPUMaterialData);
    }

    TArray<Meshlet> m_meshlets{};       // 1体のメッシュレット配列


    TArray<UInt32> m_vertex_indices{};  // 1体のモデルにおける頂点配列
    TArray<UInt32> m_triangle_indices{};// 1体のモデルにおける3角形配列

    const size_t max_vertices = 64;
    const size_t max_primitives = 126;

    // 
    const float* mesh_vertices = model->GetPositions()[0].data();
    size_t mesh_vertex_count = model->GetPositions().size();

    for (const auto& submesh : model->GetSubMeshHandles())
    {
        // このサブメッシュが使用しているインデックス範囲だけを指すポインタを作る
        const unsigned int* submesh_indices = model->GetIndices().data() + submesh.index_offset;
        size_t submesh_index_count = submesh.index_count;

        // このサブメッシュに必要な最大の Meshlet 数を計算
        size_t max_meshlets = meshopt_buildMeshletsBound(submesh_index_count, max_vertices, max_primitives);

        // 先にこれくらいつくられるかもで用意しておく
        TArray<meshopt_Meshlet> local_meshlets(max_meshlets);
        TArray<UInt32> local_vertices(max_meshlets * max_vertices);
        TArray<UInt8> local_triangles(max_meshlets * max_primitives * 3);

        // メッシュレットの構築
        size_t local_count = meshopt_buildMeshlets(
            local_meshlets.data(), local_vertices.data(), local_triangles.data(),
            submesh_indices, submesh_index_count,
            mesh_vertices, mesh_vertex_count, sizeof(Vector3),
            max_vertices, max_primitives, 0.5f
        );

		local_meshlets.resize(local_count);
		local_vertices.resize(local_count * max_vertices);
		local_triangles.resize(local_count * max_primitives * 3);


        // メッシュ内のoffsetを
        UInt32 submesh_vert_idx_offset = static_cast<UInt32>(m_vertex_indices.size());
        UInt32 submesh_tri_idx_offset = static_cast<UInt32>(m_triangle_indices.size());

        size_t cc = 0;

        // 有効なメッシュレットのみ登録
        for (size_t i = 0; i < local_count; ++i)
        {
            const auto& meshlet = local_meshlets[i];
            Meshlet l = {};
            l.vertex_offset = submesh_vert_idx_offset + meshlet.vertex_offset;
            l.vertex_count = meshlet.vertex_count;
            l.triangle_offset = submesh_tri_idx_offset + (meshlet.triangle_offset / 3);
            l.triangle_count = meshlet.triangle_count;
            l.material_index = global_material_index + submesh.material_index;

            m_meshlets.push_back(l);
            cc += l.triangle_count;
        }

        TArray<UInt32> local_packed_triangles;
        local_packed_triangles.reserve(cc);

        // 有効な三角形数（1プリミティブ = 3つのインデックス）の分だけパッキング
        for (size_t i = 0; i < cc; ++i)
        {
            size_t tri_idx = i * 3;
            UInt32 packed = 0;
            packed |= (static_cast<UInt32>(local_triangles[tri_idx + 0] & 0xff) << 0);  // 1番目の頂点ID
            packed |= (static_cast<UInt32>(local_triangles[tri_idx + 1] & 0xff) << 8);  // 2番目の頂点ID
            packed |= (static_cast<UInt32>(local_triangles[tri_idx + 2] & 0xff) << 16); // 3番目の頂点ID

            local_packed_triangles.push_back(packed);
        }

        m_vertex_indices.insert(m_vertex_indices.end(), local_vertices.begin(), local_vertices.end());
        m_triangle_indices.insert(m_triangle_indices.end(), local_packed_triangles.begin(), local_packed_triangles.end());
    }

    UInt64 vertex_offset = 0;
    UInt64 triangle_offset = 0;

    {
        /*
        * m_vertex_indicesの中身に対してglobal_vertex_offsetを加算した値が全体の頂点バッファのオフセットになる
        */
        UInt64 offset = 0;
        UInt32 num = m_vertex_indices.size();
        vertex_indices_buffer->Allocate(num, &offset);
        RenderCore::Get().UploadBuffer(vertex_indices_buffer->GetBuffer(), offset, m_vertex_indices.data(), sizeof(UInt32) * num);
        /*
		* meshletのoffsetは頂点バッファのオフセットではなく、頂点インデックスバッファのオフセットを指す必要がある
		* そのためこのoffsetをmeshletのvertex_offsetに加算して設定する必要がある
        */
        vertex_offset = offset / sizeof(UInt32);

    }
    {
        UInt64 offset = 0;
        UInt32 num = m_triangle_indices.size();
        triangle_indices_buffer->Allocate(num, &offset);
        RenderCore::Get().UploadBuffer(triangle_indices_buffer->GetBuffer(), offset, m_triangle_indices.data(), sizeof(UInt32) * num);
        /*
        * meshletのtriangle_offsetは三角形インデックスバッファのオフセットを指す必要がある
        * そのためこのoffsetをmeshletのtriangle_offsetに加算して設定する必要がある
        */
        triangle_offset = offset / sizeof(UInt32);
    }

    {
        for (auto& meshlet : m_meshlets)
        {
			meshlet.vertex_offset += static_cast<UInt32>(vertex_offset);
			meshlet.triangle_offset += static_cast<UInt32>(triangle_offset);
        }

        UInt64 offset = 0;
        UInt32 num = m_meshlets.size();
        meshlet_buffer->Allocate(num, &offset);
        RenderCore::Get().UploadBuffer(meshlet_buffer->GetBuffer(), offset, m_meshlets.data(), sizeof(Meshlet) * num);

        //このoffsetはメッシュが使用しないとわかって削除された時に返す用でもっておく
    }


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

    struct MeshConstants
    {
        float4x4 WorldViewProj;
        UInt32 VertexHeapIndex; // 元の頂点データのバッファ
		UInt32 VertexAttributeHeapIndex; // 元の頂点属性データのバッファ
        UInt32 MeshletMetaHeapIndex; // global_meshlets バッファ
        UInt32 MeshletVertHeapIndex; // global_meshlet_vertices バッファ
        UInt32 MeshletTriHeapIndex; // global_meshlet_triangles バッファ
        UInt32 MaterialHeapIndex;
        UInt32 GlobalVertexPositionOffset;
        UInt32 GlobalVertexAttributeOffset;

        UInt32 VertexStride; // 
        UInt32 AttributeFlags; // 
    };

    static float3 camera_pos = Vector3(0.0f, 10.0f, -25.0f);


    auto list = RenderCore::Get().GetCommandList();
    while (WindowSystem::Get().ProcessMessages() && !WindowSystem::Get().ShouldClose())
    {
        RenderCore::Get().BeginFrame();
        list->SetDescriptorHeap(RenderCore::Get().GetResourceHeap(), RenderCore::Get().GetSamplerHeap());

        list->SetGraphicsPipeline(pipeline.get());
        auto nativeCmdList = static_cast<D3D12CommandBuffer*>(list->GetRHI())->GetNative();


        if (GetKeyState('W') & 0x8000) { camera_pos.Z() += 0.2f; }
        if (GetKeyState('S') & 0x8000) { camera_pos.Z() -= 0.2f; }
        if (GetKeyState('A') & 0x8000) { camera_pos.X() -= 0.2f; }
        if (GetKeyState('D') & 0x8000) { camera_pos.X() += 0.2f; }

        Camera camera = {};
        camera.SetPosition(camera_pos);

        MeshConstants constants = {};
        constants.WorldViewProj = camera.GetViewProjectionMatrix();
        constants.VertexHeapIndex = vertex_buffer->GetBuffer()->GetSRVHandle().index;
        constants.VertexAttributeHeapIndex = vertex_attribute_buffer->GetBuffer()->GetSRVHandle().index;
        constants.MeshletMetaHeapIndex = meshlet_buffer->GetBuffer()->GetSRVHandle().index;
        constants.MeshletVertHeapIndex = vertex_indices_buffer->GetBuffer()->GetSRVHandle().index;
        constants.MeshletTriHeapIndex = triangle_indices_buffer->GetBuffer()->GetSRVHandle().index;
        constants.MaterialHeapIndex = material_buffer->GetBuffer()->GetSRVHandle().index;

		constants.GlobalVertexPositionOffset = static_cast<UInt32>(global_vertex_offset);
		constants.GlobalVertexAttributeOffset = static_cast<UInt32>(global_vertex_attribute_offset);

        constants.VertexStride = model->GetVertexAttributeStride();
        constants.AttributeFlags = model->GetVertexAttributeFlags().GetValue();

        {
            auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<MeshConstants>(&constants);

            list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 0);
        }

		list->SetGraphicsSamplerState(RenderCore::Get().GetPresetSampler(ESamplerPresetType::LinearClamp), 1);
        struct TestConstants
        {
            UInt32 MaterialHeapIndex; //
            uint3 padding;
        };

        {
            TestConstants tc = {};
            tc.MaterialHeapIndex = material_buffer->GetBuffer()->GetSRVHandle().index;
            auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<TestConstants>(&tc);

            list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);
        }

        auto count = m_meshlets.size();

        nativeCmdList->DispatchMesh(count, 1, 1);

        RenderContext ctx = {};
        ctx.command_list = RenderCore::Get().GetCommandList();
        ctx.transient_buffer = RenderCore::Get().GetTransientAllocator();

		GUISystem::Get().BeginFrame();

        ImGui::Begin("Texture Viewer");

        // 1枚あたりの表示サイズ（見やすい大きさに調整してください）
        ImVec2 texture_size(320.0f, 240.0f);

        // ImGuiのスタイルから、画像同士の横の隙間（間隔）を取得
        float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
        float spacing = ImGui::GetStyle().ItemSpacing.x;

        for (auto& [id, tex_ptr] : m_texture_cache)
        {
            auto tex = tex_ptr.get();
            if (!tex) continue;

            // 1枚のテクスチャを描画
            ImGui::Image(tex, texture_size);

            // 次の画像が現在のウィンドウの右端をはみ出すかチェック
            float last_button_x2 = ImGui::GetItemRectMax().x;
            float next_button_x2 = last_button_x2 + spacing + texture_size.x;

            // まだ右側にスペースがあれば横に並べる。なければ自動で次の行（列）へ
            if (next_button_x2 < window_visible_x2)
            {
                ImGui::SameLine();
            }
        }

        ImGui::End();

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