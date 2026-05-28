#include "D3D12SamplerState.h"
#include "D3D12DescriptorHeap.h"

#include "RHI/Definitions.h"

#include "Core/Assertion.h"

namespace
{
    using namespace Alpha;

    static D3D12_FILTER GetD3D12Filter(ERHIFilter filter)
    {
        switch (filter)
        {
        case ERHIFilter::Point:             return D3D12_FILTER_MIN_MAG_MIP_POINT;
        case ERHIFilter::Linear:            return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        case ERHIFilter::Anisotropic:       return D3D12_FILTER_ANISOTROPIC;
        case ERHIFilter::ComparisonLinear:  return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        default:                            return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        }

        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    }

    static D3D12_TEXTURE_ADDRESS_MODE GetD3D12AddressMode(ERHIAddressMode mode)
    {
        switch (mode)
        {
        case ERHIAddressMode::Wrap:   return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        case ERHIAddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        case ERHIAddressMode::Clamp:  return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        case ERHIAddressMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        default:                      return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        }
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    }

    static D3D12_COMPARISON_FUNC GetD3D12ComparisonFunc(ERHIComparisonFunc func)
    {
        switch (func)
        {
        case ERHIComparisonFunc::Never:     return D3D12_COMPARISON_FUNC_NEVER;
        case ERHIComparisonFunc::Less:      return D3D12_COMPARISON_FUNC_LESS;
        case ERHIComparisonFunc::Equal:     return D3D12_COMPARISON_FUNC_EQUAL;
        case ERHIComparisonFunc::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
        case ERHIComparisonFunc::Always:    return D3D12_COMPARISON_FUNC_ALWAYS;
        default:                            return D3D12_COMPARISON_FUNC_NEVER;
        }
        return D3D12_COMPARISON_FUNC_NEVER;
    }
}

namespace Alpha
{
	D3D12SamplerState::D3D12SamplerState(D3D12Device* device)
		: RHISamplerState()
		, D3D12DeviceChild(device)
	{
	}

	D3D12SamplerState::~D3D12SamplerState()
	{
	}

	bool D3D12SamplerState::Initialize(const RHISamplerStateDesc& desc)
	{
		auto device = GetD3D12Device();

		auto d3d12_heap = static_cast<D3D12DescriptorHeap*>(desc.descriptor_heap);

		if (ALPHA_CHECK_ERROR((d3d12_heap->GetType() != ERHIDescriptorType::Sampler), L"SamplerState用のデスクリプタヒープではありません"))
            return false;


		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = d3d12_heap->GetCPUHandle(desc.heap_index);
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = d3d12_heap->GetGPUHandle(desc.heap_index);

        D3D12_SAMPLER_DESC sampler_desc = {};
        sampler_desc.Filter = GetD3D12Filter(desc.filter);
        sampler_desc.AddressU = GetD3D12AddressMode(desc.address_u);
        sampler_desc.AddressV = GetD3D12AddressMode(desc.address_v);
        sampler_desc.AddressW = GetD3D12AddressMode(desc.address_w);

        sampler_desc.MipLODBias = desc.mip_lod_bias;
        sampler_desc.MaxAnisotropy = desc.max_anisotropy;
        sampler_desc.ComparisonFunc = GetD3D12ComparisonFunc(desc.comparison_func);

        for (int i = 0; i < 4; ++i) 
        {
            sampler_desc.BorderColor[i] = desc.border_color[i];
        }

        sampler_desc.MinLOD = desc.min_lod;
        sampler_desc.MaxLOD = desc.max_lod;

		device->CreateSampler(&sampler_desc, cpu_handle);

        m_cpu_handle = cpu_handle;
        m_gpu_handle = gpu_handle;

		return true;
	}
}