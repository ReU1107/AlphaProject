#include "D3D12PipelineLayout.h"
#include "RHI/Definitions.h"

#if RHI_D3D12

namespace
{
	D3D12_DESCRIPTOR_RANGE_TYPE GetD3D12DescriptorRangeType(Alpha::ERHIResourceViewType type)
	{
		switch (type)
		{
		case Alpha::ERHIResourceViewType::ShaderResourceView	: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case Alpha::ERHIResourceViewType::UnorderedAccessView	: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		case Alpha::ERHIResourceViewType::ConstantBufferView	: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case Alpha::ERHIResourceViewType::SamplerView			: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		default:
			break;
		}
		return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	}

	static D3D12_ROOT_PARAMETER_TYPE GetD3D12RootParameterType(Alpha::ERHIResourceViewType type)
	{
		switch (type)
		{
		case Alpha::ERHIResourceViewType::ConstantBufferView	: return D3D12_ROOT_PARAMETER_TYPE_CBV; 
		case Alpha::ERHIResourceViewType::ShaderResourceView	: return D3D12_ROOT_PARAMETER_TYPE_SRV; 
		case Alpha::ERHIResourceViewType::UnorderedAccessView	: return D3D12_ROOT_PARAMETER_TYPE_UAV;
		default:
			break;
		}
		return D3D12_ROOT_PARAMETER_TYPE_SRV;
	}
}

namespace Alpha
{
	D3D12PipelineLayout::D3D12PipelineLayout(D3D12Device* device)
		: RHIPipelineLayout()
		, D3D12DeviceChild(device)
	{
	}

	D3D12PipelineLayout::~D3D12PipelineLayout()
	{
	}

	bool D3D12PipelineLayout::Initialize(const RHIPipelineLayoutDesc& desc)
	{
		UInt32 parameter_num = (UInt32)desc.parameters.size();

		TArray<D3D12_ROOT_PARAMETER1> parameters(desc.parameters.size());
		TArray<D3D12_DESCRIPTOR_RANGE1*> temp_ranges;

		for (UInt32 parameter_index = 0; parameter_index < parameter_num; parameter_index++)
		{
			auto& src = desc.parameters[parameter_index];
			auto& dst = parameters[parameter_index];

			switch (src.parameter_type)
			{
			case ERHIParameterType::RootConstants:
				dst.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
				dst.Constants.ShaderRegister = src.register_index;
				dst.Constants.Num32BitValues = src.range_num;
				dst.Constants.RegisterSpace = 0;
				break;
			case ERHIParameterType::RootDescriptors:
				dst.ParameterType = GetD3D12RootParameterType(src.view_type);
				dst.Descriptor.ShaderRegister = src.register_index;
				dst.Descriptor.RegisterSpace = 0;
				dst.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE;
				break;
			case ERHIParameterType::DescriptorTable:
			{
				auto ranges = new D3D12_DESCRIPTOR_RANGE1[src.range_num];

				dst.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				dst.DescriptorTable.NumDescriptorRanges = src.range_num;
				dst.DescriptorTable.pDescriptorRanges = ranges;
				dst.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

				for (UInt32 range_index = 0; range_index < src.range_num; range_index++)
				{
					const RHIDescriptorRange& src_range = src.ranges[range_index];
					D3D12_DESCRIPTOR_RANGE1& dst_range = ranges[range_index];
					// メモ用としてのコメント
					// このレンジはこのビューを
					dst_range.RangeType = GetD3D12DescriptorRangeType(src_range.view_type);
					// この数だけ連続してDescriptorHeapで登録します
					dst_range.NumDescriptors = src_range.resource_num;
					// 一番最初の番号はこれ
					dst_range.BaseShaderRegister = src_range.register_index;
					dst_range.RegisterSpace = 0;
					dst_range.Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;
					dst_range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				}
				temp_ranges.push_back(ranges);
			}	break;
			default:
				break;
			}
		}
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC versioned_desc = {};
		versioned_desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
		versioned_desc.Desc_1_1.NumParameters = (UINT)parameters.size();
		versioned_desc.Desc_1_1.pParameters = parameters.data();
		versioned_desc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;


		if (EnumHasFlag(desc.flags, ERHIPipelineLayoutFlags::InputAssembler))
			versioned_desc.Desc_1_1.Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		if (EnumHasFlag(desc.flags, ERHIPipelineLayoutFlags::Bindless))
		{
			versioned_desc.Desc_1_1.Flags |=
				D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
				D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
		}

		ComPtr<ID3DBlob> signature_blob;
		ComPtr<ID3DBlob> error_blob;
		
		HRESULT hr = D3D12SerializeVersionedRootSignature(&versioned_desc, &signature_blob, &error_blob);
		if (ALPHA_CHECK_ERROR(FAILED(hr), L"ルートシグネチャの作成に失敗しました"))
		{
			for (auto& range : temp_ranges)
			{
				delete[] range;
			}
			return false;
		}
		auto device = GetD3D12Device();

		hr = device->CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&m_root_signature));
		if (ALPHA_CHECK_ERROR(FAILED(hr), L"ルートシグネチャの作成に失敗しました"))
		{
			for (auto& range : temp_ranges)
			{
				delete[] range;
			}
			return false;
		}

		for (auto& range : temp_ranges)
		{
			delete[] range;
		}

		return true;
	}
}

#endif

/*
* 	struct RHIDescriptorRange 
	{
		UInt32 resource_count{ 0 };		// 同じリソースが連続する場合の数↓だと 8
		UInt32 register_index{ 0 };		// 開始時の番号 Texture2D tex[8] : register(t5); なら5
		ERHIResourceViewType view_type;
	};
	メモ
	Texture2D diffuse : register(t0);
	Texture2D albedo : register(t1);
	Texture2D emissive : register(t2);

	hlsl側でこういった感じで宣言した場合に
	register_index = 0; 一番最初の0
	そこから3枚のテクスチャをDescriptorHeap側で連続して登録しておいて
	resource_count = 3;
	としておく
*/