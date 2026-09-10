#include "D3D12CommandLayout.h"
#include "RHI/Definitions.h"
#if RHI_D3D12

namespace
{
    Alpha::UInt32 GetIndirectArgumentSize(Alpha::ERHIIndirectArgType type)
    {
        switch (type)
        {
        case Alpha::ERHIIndirectArgType::Draw:         return sizeof(D3D12_DRAW_ARGUMENTS);         
        case Alpha::ERHIIndirectArgType::DrawIndexed:  return sizeof(D3D12_DRAW_INDEXED_ARGUMENTS); 
        case Alpha::ERHIIndirectArgType::Dispatch:     return sizeof(D3D12_DISPATCH_ARGUMENTS);     
        case Alpha::ERHIIndirectArgType::DispatchMesh: return sizeof(D3D12_DISPATCH_MESH_ARGUMENTS);
        case Alpha::ERHIIndirectArgType::VertexBuffer: return sizeof(D3D12_VERTEX_BUFFER_VIEW);     
        case Alpha::ERHIIndirectArgType::IndexBuffer:  return sizeof(D3D12_INDEX_BUFFER_VIEW);      
        case Alpha::ERHIIndirectArgType::Constant:     return sizeof(Alpha::UInt32);                
        default: return 0;
        }
    }
}

namespace Alpha
{
	D3D12CommandLayout::D3D12CommandLayout(D3D12Device* device)
		: RHICommandLayout()
		, D3D12DeviceChild(device) 
	{
	}

	D3D12CommandLayout::~D3D12CommandLayout()
	{
	}

	bool D3D12CommandLayout::Initialize(const RHICommandLayoutDesc& desc)
	{
		auto device = GetD3D12Device();
        std::vector<D3D12_INDIRECT_ARGUMENT_DESC> d3d12_args(desc.arguments.size());
        UInt32 calculated_stride = 0;

        for (size_t i = 0; i < desc.arguments.size(); ++i)
        {
            auto& src = desc.arguments[i];
            auto& dst = d3d12_args[i];

            switch (src.type)
            {
            case ERHIIndirectArgType::Draw:
                dst.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;
                calculated_stride += sizeof(D3D12_DRAW_ARGUMENTS);
                break;

            case ERHIIndirectArgType::DrawIndexed:
                dst.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
                calculated_stride += sizeof(D3D12_DRAW_INDEXED_ARGUMENTS);
                break;

            case ERHIIndirectArgType::Dispatch:
                dst.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH;
                calculated_stride += sizeof(D3D12_DISPATCH_ARGUMENTS);
                break;

            case ERHIIndirectArgType::Constant:
                dst.Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
                dst.Constant.RootParameterIndex = src.root_parameter_index;
                dst.Constant.DestOffsetIn32BitValues = src.dest_runtime_constant_offset;
                dst.Constant.Num32BitValuesToSet = src.num_32bit_values_to_set;
                calculated_stride += sizeof(UInt32) * src.num_32bit_values_to_set;
                break;
            case ERHIIndirectArgType::DispatchMesh:
                dst.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DISPATCH_MESH;
                calculated_stride += sizeof(D3D12_DISPATCH_MESH_ARGUMENTS);
                break;
            default:
                return false;
            }
        }
        /*
        * 一旦rootsignatureを使うやつ(viewを変更する系)は必要になったら追加する
        */
        D3D12_COMMAND_SIGNATURE_DESC signature_desc = {};
        signature_desc.ByteStride = (desc.stride > 0) ? desc.stride : calculated_stride;
        signature_desc.NumArgumentDescs = (UINT)d3d12_args.size();
        signature_desc.pArgumentDescs = d3d12_args.data();
        signature_desc.NodeMask = 0;

        HRESULT hr = device->CreateCommandSignature(&signature_desc, nullptr, IID_PPV_ARGS(&m_signature));

        if (ALPHA_CHECK_ERROR(SUCCEEDED(hr), L"コマンドシグネチャの作成に失敗しました"))
        {
            return false;
        }

        return true;
	}
}

#endif