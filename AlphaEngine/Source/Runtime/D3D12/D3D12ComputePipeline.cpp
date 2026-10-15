#include "D3D12ComputePipeline.h"
#include "D3D12Shader.h"
#include "D3D12PipelineLayout.h"
#include "RHI/Definitions.h"
#include "D3D12Util.h"
#if RHI_D3D12

namespace Alpha
{
	D3D12ComputePipeline::D3D12ComputePipeline(D3D12Device* device)
		: RHIComputePipeline()
		, D3D12DeviceChild(device)
	{
	}

	D3D12ComputePipeline::~D3D12ComputePipeline()
	{
	}

	bool D3D12ComputePipeline::Initialize(const RHIComputePipelineDesc& desc)
	{
        auto device = GetD3D12Device();
        auto pipeline_layout = static_cast<D3D12PipelineLayout*>(desc.layout);
        auto compute_shader = static_cast<D3D12Shader*>(desc.compute_shader);

        std::vector<uint8_t> stream_data;
        stream_data.reserve(256);

        PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE, ID3D12RootSignature*> rs_sub;
        rs_sub.data = pipeline_layout->GetNativeHandle();
        AddSubobject(stream_data, rs_sub);

        PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS, D3D12_SHADER_BYTECODE> cs_sub;
        cs_sub.data = { compute_shader->GetBytecodePointer(), compute_shader->GetBytecodeLength() };
        AddSubobject(stream_data, cs_sub);

        PipelineStateSubobject<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK, UINT> node_sub;
        node_sub.data = 0;
        AddSubobject(stream_data, node_sub);

        D3D12_PIPELINE_STATE_STREAM_DESC stream_desc = {};
        stream_desc.SizeInBytes = stream_data.size();
        stream_desc.pPipelineStateSubobjectStream = stream_data.data();

        HRESULT hr = device->CreatePipelineState(&stream_desc, IID_PPV_ARGS(&m_pipeline_state));
        if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"コンピュートパイプラインの作成に失敗しました"))
        {
            return false;
        }
        return true;
	}
}
#endif