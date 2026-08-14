#include "Resource.hlsli"


ConstantBuffer<SystemBufferIndices> buffer_indices : register(b0);
ConstantBuffer<FrameRenderContext> frame : register(b1);

struct PassConstants
{
    uint result_index;
    uint volumetric;
    uint transmittance;
    uint scene_base;
};

ConstantBuffer<PassConstants> pass_info : register(b2);

[numthreads(8, 8, 1)]
void main(uint3 dispatch_index : SV_DispatchThreadID)
{
    // それぞれが単品で確認できるようにfloat4にしてる
    RWTexture2D<float4> result = ResourceDescriptorHeap[pass_info.result_index];
    Texture2D<float4> volumetric_tex = ResourceDescriptorHeap[pass_info.volumetric];
    Texture2D<float4> transmittance_tex = ResourceDescriptorHeap[pass_info.transmittance];
    Texture2D<float4> scene_base_tex = ResourceDescriptorHeap[pass_info.scene_base];
    
    SamplerState sam = SamplerDescriptorHeap[SAMPLER_PRESET_LINEAR_CLAMP];

    uint2 index = dispatch_index.xy;

    uint2 thread_dimension;
    result.GetDimensions(thread_dimension.x, thread_dimension.y);

    float2 uv = ((float2) dispatch_index + 0.5f) / thread_dimension;
    
    
    float3 volumetric = volumetric_tex.Sample(sam, uv).rgb;
    float3 transmittance = transmittance_tex.Sample(sam, uv).rgb;
    float3 scene_base = scene_base_tex.Sample(sam, uv).rgb;
    
    
    result[index] = float4(volumetric + transmittance * scene_base, 1.0f);
    
}