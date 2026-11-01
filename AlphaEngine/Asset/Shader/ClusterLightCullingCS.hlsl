#include "Resource.hlsli"

struct PassConstants
{
    uint cluster;
    uint light_indices;
    uint result;
};

ConstantBuffer<PassConstants> pass_info : register(b2);



float3 FroxelUVWToWorldPosition(float3 uvw, float4x4 inv_view_proj, float3 camera_pos, float near, float far)
{
    // フラスタムにおける一番近い面のとある一点

    float2 clip_space_pos = float2(uvw.xy) * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 ndc_pos = float4(clip_space_pos, 1.0f, 1.0f);
    
    float4 near_world_pos = mul(inv_view_proj, ndc_pos);

    near_world_pos /= near_world_pos.w; // w除算

    // カメラから一番近い面までのベクトル
    float3 view_vector = near_world_pos.xyz - camera_pos;

    // 
    float linear_depth = near * pow(far / near, uvw.z);
    float depth_ratio = linear_depth / near;
    
    return camera_pos + view_vector * depth_ratio;
}

bool IntersectAABBSphere(float3 aabb_min, float3 aabb_max, float3 center, float radius)
{
    float3 closest_point = max(aabb_min, min(center, aabb_max));
    
    float3 d = center - closest_point;
    float distance_sq = dot(d, d);
    
    return distance_sq <= (radius * radius);
}

// とりあえずクラスターごとの最大数を
static const uint MAX_LIGHT_COUNT = 64;

[numthreads(8, 8, 4)]
void main(uint3 dispatch_index : SV_DispatchThreadID)
{
    RWTexture3D<uint> result = ResourceDescriptorHeap[pass_info.result];
    
    StructuredBuffer<LightData> light_buffer = ResourceDescriptorHeap[render_buffers.light];
    
    RWStructuredBuffer<Cluster> cluster_buffer = ResourceDescriptorHeap[pass_info.cluster];
    RWStructuredBuffer<uint> light_indices_buffer = ResourceDescriptorHeap[pass_info.light_indices];

    
    uint3 thread_dimension;
    result.GetDimensions(thread_dimension.x, thread_dimension.y, thread_dimension.z);

    if (any(dispatch_index >= thread_dimension))
        return;
    
    static const float near_z = 0.1f;
    static const float far_z = 1000.0f;
    
    // クラスタの左端と右端
    float2 uv_min = float2(dispatch_index.xy) / float2(thread_dimension.xy);
    float2 uv_max = float2(dispatch_index.xy + 1.0f) / float2(thread_dimension.xy);
    
    // クラスタの手前と奥それぞれの4頂点
    float w_min = float(dispatch_index.z) / thread_dimension.z;

    float w_max = ((float) dispatch_index.z + 1.0f) / thread_dimension.z;
  
    float3 p0 = FroxelUVWToWorldPosition(float3(uv_min.x, uv_min.y, w_min), render_frame.inv_view_proj, render_frame.camera_pos.xyz, near_z, far_z);
    float3 p1 = FroxelUVWToWorldPosition(float3(uv_max.x, uv_min.y, w_min), render_frame.inv_view_proj, render_frame.camera_pos.xyz, near_z, far_z);
    float3 p2 = FroxelUVWToWorldPosition(float3(uv_min.x, uv_max.y, w_min), render_frame.inv_view_proj, render_frame.camera_pos.xyz, near_z, far_z);
    float3 p3 = FroxelUVWToWorldPosition(float3(uv_max.x, uv_max.y, w_min), render_frame.inv_view_proj, render_frame.camera_pos.xyz, near_z, far_z);

    float3 p4 = FroxelUVWToWorldPosition(float3(uv_min.x, uv_min.y, w_max), render_frame.inv_view_proj, render_frame.camera_pos.xyz, near_z, far_z);
    float3 p5 = FroxelUVWToWorldPosition(float3(uv_max.x, uv_min.y, w_max), render_frame.inv_view_proj, render_frame.camera_pos.xyz, near_z, far_z);
    float3 p6 = FroxelUVWToWorldPosition(float3(uv_min.x, uv_max.y, w_max), render_frame.inv_view_proj, render_frame.camera_pos.xyz, near_z, far_z);
    float3 p7 = FroxelUVWToWorldPosition(float3(uv_max.x, uv_max.y, w_max), render_frame.inv_view_proj, render_frame.camera_pos.xyz, near_z, far_z);

    float3 aabb_min = min(min(min(p0, p1), min(p2, p3)), min(min(p4, p5), min(p6, p7)));
    float3 aabb_max = max(max(max(p0, p1), max(p2, p3)), max(max(p4, p5), max(p6, p7)));
    
    uint cluster_index = (dispatch_index.z * thread_dimension.y + dispatch_index.y) * thread_dimension.x + dispatch_index.x;
    
  
    uint light_count = 0;

    uint light_element_offset = cluster_index * MAX_LIGHT_COUNT;
    
    for (uint i = 0; i < render_frame.light_count; ++i)
    {
        LightData light = light_buffer[i];
        
        // ディレクショナルは無視
        if (light.type == LIGHT_DIRECTIONAL)
            continue;
        
        if (light_count >= MAX_LIGHT_COUNT)
            continue;

        if (!IntersectAABBSphere(aabb_min, aabb_max, light.position, light.radius))
            continue;

        light_indices_buffer[light_element_offset + light_count] = i;
        
        light_count++;
        
    }
    
    // pixでの確認用
    for (uint i = light_count; i < MAX_LIGHT_COUNT; ++i)
    {
        light_indices_buffer[light_element_offset + i] = INVALID_INDEX;
    }
    
    Cluster cluster = (Cluster) 0;
    cluster.offset = light_element_offset;
    cluster.count = light_count;
    
    cluster_buffer[cluster_index] = cluster;
    
    result[dispatch_index] = light_count;
}