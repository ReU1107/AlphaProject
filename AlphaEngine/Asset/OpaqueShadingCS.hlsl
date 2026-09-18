#include "Resource.hlsli"


struct PassConstants
{
    uint result_index;
    uint visibility;
    uint depth;
};

ConstantBuffer<PassConstants> pass_info : register(b2);

[numthreads(8, 8, 1)]
void main(uint3 dispatch_index : SV_DispatchThreadID)
{
    RWTexture2D<float4> result = ResourceDescriptorHeap[pass_info.result_index];
    Texture2D<float4> visibility_tex = ResourceDescriptorHeap[pass_info.visibility];
    Texture2D<float> depth_tex = ResourceDescriptorHeap[pass_info.depth];
    
    SamplerState sam = SamplerDescriptorHeap[SAMPLER_PRESET_LINEAR_CLAMP];

    uint2 index = dispatch_index.xy;

    uint2 thread_dimension;
    result.GetDimensions(thread_dimension.x, thread_dimension.y);

    float2 uv = ((float2) dispatch_index + 0.5f) / thread_dimension;
    
    // ここでvbufferから情報を復元してライティング
    // とりあえずvisibilitypassで色情報を渡してるからそのまま流す
    
    float3 color = visibility_tex.Sample(sam, uv).rgb;

    result[index] = float4(color, 1.0f);
}