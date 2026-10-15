#include "Resource.hlsli"

// 空のPayload（テスト用）
struct [raypayload] RayPayload
{
    float3 color : write(miss, closesthit) : read(caller);
};

struct PassConstants
{
    uint result_index;
};

ConstantBuffer<PassConstants> pass_info : register(b2);

[shader("raygeneration")]
void MyRayGen()
{
    uint2 index = DispatchRaysIndex().xy;

    float2 uv = ((float2) index + 0.5f) / float2(DispatchRaysDimensions().xy);
    
    float2 clipSpacePos = uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);

    float4 targetWorldPos = mul(render_frame.inv_view_proj, float4(clipSpacePos, 0.0f, 1.0f));
    targetWorldPos.xyz /= targetWorldPos.w; 
    float3 origin = render_frame.camera_pos.xyz;
    float3 direction = normalize(targetWorldPos.xyz - origin);

    RayDesc ray;
    ray.Origin = origin;
    ray.Direction = direction;
    ray.TMin = 0.001;
    ray.TMax = 1000.0;

    RayPayload payload;
    payload.color = float3(0, 0, 0);

    RaytracingAccelerationStructure tlas = ResourceDescriptorHeap[render_buffers.rt_structure];
    RWTexture2D<float4> outputTexture = ResourceDescriptorHeap[pass_info.result_index];
    
    // レイを飛ばすテスト
    TraceRay(tlas, 0, 0xFF, 0, 0, 0, ray, payload);
    
    outputTexture[index.xy] = float4(payload.color, 1.0f);

}
