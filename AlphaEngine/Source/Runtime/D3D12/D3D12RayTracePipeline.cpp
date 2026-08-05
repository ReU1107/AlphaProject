#include "D3D12RayTracePipeline.h"
#include "D3D12PipelineLayout.h"
#include "D3D12Shader.h"

#include "RHI/Definitions.h"

#include <d3dx12.h>
#include <unordered_set>


#if RHI_D3D12


namespace Alpha
{
	D3D12RayTracePipeline::D3D12RayTracePipeline(D3D12Device* device)
		: RHIRayTracePipeline()
		, D3D12DeviceChild(device)
	{
	}

	D3D12RayTracePipeline::~D3D12RayTracePipeline()
	{
	}

	bool D3D12RayTracePipeline::Initialize(const RHIRayTracePipelineDesc& desc)
	{
        CD3DX12_STATE_OBJECT_DESC state_object_desc{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

        for (RHIShader* shader : desc.libraries)
        {
            if(!shader)
                continue;

            auto d3d12_shader = static_cast<D3D12Shader*>(shader);

            auto lib = state_object_desc.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
            D3D12_SHADER_BYTECODE bytecode{ d3d12_shader->GetBytecodePointer(), d3d12_shader->GetBytecodeLength() };
            lib->SetDXILLibrary(&bytecode);

            for (const String& entry_point : d3d12_shader->GetEntryPoints())
            {
                lib->DefineExport(entry_point.c_str());
            }

        }

        // HitGroup
        for (const RHIRayTraceHitGroupDesc& hit_group : desc.hit_groups)
        {

            auto hg = state_object_desc.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
            hg->SetHitGroupExport(hit_group.name.c_str());
            hg->SetHitGroupType(hit_group.type == ERHIHitGroupType::Triangles
                ? D3D12_HIT_GROUP_TYPE_TRIANGLES
                : D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE);

            if (!hit_group.any_hit_entry_name.IsEmpty())
                hg->SetAnyHitShaderImport(hit_group.any_hit_entry_name.c_str());

            if (!hit_group.closest_hit_entry_name.IsEmpty())
                hg->SetClosestHitShaderImport(hit_group.closest_hit_entry_name.c_str());

            if (!hit_group.intersection_entry_name.IsEmpty())
                hg->SetIntersectionShaderImport(hit_group.intersection_entry_name.c_str());
        }

        // Shader Config（Payload/Attributeサイズ）
        auto shader_config = state_object_desc.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
        shader_config->Config(desc.max_payload_size, desc.max_attribute_size);

        // Pipeline Config（最大再帰深度）
        auto pipeline_config = state_object_desc.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
        pipeline_config->Config(desc.max_recursion_depth);

        // Global Root Signature
        auto pipeline_layout = static_cast<D3D12PipelineLayout*>(desc.global_layout);
        auto global_root_sig = state_object_desc.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
        global_root_sig->SetRootSignature(pipeline_layout->GetNativeHandle());

        auto device = GetD3D12Device();


        HRESULT hr = device->CreateStateObject(state_object_desc, IID_PPV_ARGS(&m_state_object));
        if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"レイトレーシングパイプラインの作成に失敗しました"))
        {
            return false;
        }
        hr = m_state_object->QueryInterface(IID_PPV_ARGS(&m_state_object_props));
        if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"レイトレーシングパイプラインの作成に失敗しました"))
        {
            return false;
        }


		return true;
	}

    void* D3D12RayTracePipeline::GetShaderIdentifier(const StringView export_nmae) const
    {
        return m_state_object_props->GetShaderIdentifier(export_nmae.c_str());
    }
}
#endif