#include "Resource.hlsli"


struct PassConstants
{
    uint result_index;
    uint volumetric;
    uint transmittance;
    uint scene_base;
};

ConstantBuffer<PassConstants> pass_info : register(b2);

float3 ACESFilm(float3 x)
{
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;

    return saturate(
        (x * (a * x + b)) /
        (x * (c * x + d) + e)
    );
}

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
    
    float3 hdr_color = volumetric + transmittance * scene_base;
    const float exposure_ev = 0.0f;

    hdr_color *= exp2(exposure_ev);

    hdr_color = max(hdr_color, 0.0f);

    float3 ldr_color = ACESFilm(hdr_color);

    //result[index] = float4(scene_base, 1.0f);
    result[index] = float4(ldr_color, 1.0f);
    //result[index] = float4(volumetric, 1.0f);

}