#include "Resource.hlsli"

// 空のPayload（テスト用）
struct [raypayload] RayPayload
{
    float3 color : write(miss, closesthit) : read(caller);
};

ConstantBuffer<SystemBufferIndices> buffer_indices : register(b0);
ConstantBuffer<FrameRenderContext> frame : register(b1);

struct PassConstants
{
    uint result_index;
};

ConstantBuffer<PassConstants> pass_info : register(b2);

[shader("raygeneration")]
void RayGenerate()
{
    RWTexture3D<float4> result = ResourceDescriptorHeap[pass_info.result_index];
    
    uint3 index = DispatchRaysIndex();

    float u = ((float) index.x + 0.5f) / DispatchRaysDimensions().x;
    float v = ((float) index.y + 0.5f) / DispatchRaysDimensions().y;
    float w = ((float) index.z + 0.5f) / DispatchRaysDimensions().z;
    
    float near_z = 0.1f;
    float far_z = 1000.0f;
    
    float2 clip_space_pos = float2(u, v) * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 ndc_pos = float4(clip_space_pos, 0.0f, 1.0f);
    
    // 一番遠い
    float4 far_pos = mul(frame.proj_view, ndc_pos);
    far_pos /= far_pos.w; // w除算
    
    float3 view_vector = far_pos.xyz - frame.camera_pos.xyz;
    
    float linear_depth = near_z * pow(far_z / near_z, w);
    float depth_ratio = linear_depth / far_z;

    float3 world_pos = frame.camera_pos.xyz + view_vector * depth_ratio;
    
    float3 direction = normalize(float3(1.0f, 1.0f, 0.0f));
    
    RayDesc ray;
    ray.Origin = world_pos + direction * 0.001f;
    ray.Direction = direction;
    ray.TMin = 0.001;
    ray.TMax = 1000.0;

    RaytracingAccelerationStructure tlas = ResourceDescriptorHeap[buffer_indices.rt_structure];

    RayPayload payload;
    payload.color = float3(1.0, 1.0, 1.0);

    TraceRay(tlas, 0, 0xFF, 0, 0, 0, ray, payload);

    result[index] = float4(payload.color, 1.0);

}

[shader("anyhit")]
void TranslucentCheck(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    
}

[shader("closesthit")]
void TranslucentHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    payload.color = float3(0.0, 0.0, 0.0);
}

[shader("closesthit")]
void OpaqueHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    // 不透明な壁に当たったので、光の到達率を完全にゼロにする
    payload.color = float3(0.0, 0.0, 0.0);
}

[shader("miss")]
void Miss(inout RayPayload payload)
{
    payload.color = float3(1.0, 1.0, 1.0);

}

