#include "Resource.hlsli"
#include "Lighting.hlsli"

struct PassConstants
{
    uint cluster;
    uint light_cluster;
    uint light_indices;
    uint result_index;
};

ConstantBuffer<PassConstants> pass_info : register(b2);

[numthreads(8, 8, 4)]
void main(uint3 dispatch_index : SV_DispatchThreadID)
{
    RWTexture3D<float4> result = ResourceDescriptorHeap[pass_info.result_index];
    
    StructuredBuffer<LightData> light_buffer = ResourceDescriptorHeap[render_buffers.light];

    Texture3D<uint> cluster_tex = ResourceDescriptorHeap[pass_info.cluster];    
    StructuredBuffer<Cluster> cluster_buffer = ResourceDescriptorHeap[pass_info.light_cluster];
    StructuredBuffer<uint> light_indices_buffer = ResourceDescriptorHeap[pass_info.light_indices];
    
    SamplerState sam = SamplerDescriptorHeap[SAMPLER_PRESET_POINT_CLAMP];

    
    uint3 thread_dimension;
    result.GetDimensions(thread_dimension.x, thread_dimension.y, thread_dimension.z);

    if (any(dispatch_index >= thread_dimension))
        return;
    
    float3 uvw = ((float3) dispatch_index + 0.5f) / thread_dimension;
    
    float near_z = 0.1f;
    float far_z = 1000.0f;
    
    float2 clip_space_pos = float2(uvw.xy) * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 ndc_pos = float4(clip_space_pos, 0.0f, 1.0f);
    
    // 一番遠い
    float4 far_pos = mul(render_frame.inv_view_proj, ndc_pos);
    far_pos /= far_pos.w; // w除算
    
    float3 view_vector = far_pos.xyz - render_frame.camera_pos.xyz;
    
    float linear_depth = near_z * pow(far_z / near_z, uvw.z);
    float depth_ratio = linear_depth / far_z;

    float3 world_pos = render_frame.camera_pos.xyz + view_vector * depth_ratio;
    
    uvw = WorldPositionToFroxelUVW(world_pos, render_frame.view, render_frame.view_proj, 0.1f, 1000.0f);

    
    //uint cluster_index = cluster_tex.Sample(sam, uvw);

    // あとで定数バッファからもらってくるようにすｒ
    uint3 cluster_dim = uint3(32, 24, 16);

    float3 cluster_uvw = saturate(uvw);

    uint3 cluster_coord = min((uint3) (cluster_uvw * cluster_dim), cluster_dim - 1);

    uint cluster_index = (cluster_coord.z * cluster_dim.y + cluster_coord.y) * cluster_dim.x + cluster_coord.x;
    
    Cluster cluster = cluster_buffer[cluster_index];
    
    
    float3 radiance = float3(0, 0, 0);

    if (render_frame.debug_value == 6)
    {
        for (uint i = 0; i < render_frame.light_count; ++i)
        {
            LightData light = light_buffer[i];
        
            if (light.type == 2)
                continue;

            float dist = length(light.position - world_pos);
        
        // ライトの範囲外なら無視
            if (dist > light.radius)
                continue;
        
            float3 direction = (light.position - world_pos) / max(dist, 1e-6f);
        
            float attenuation = max(0.0f, 1.0f - (dist / light.radius));
        
        // スポットライトの円錐減衰
            if (light.type == 1)
            {
                float angle = dot(-direction, normalize(light.direction));
                attenuation *= saturate((angle - light.outer_cone) / (light.inner_cone - light.outer_cone));
            }

            radiance += light.color * light.intensity * attenuation;
            
        }
        
    }
    else
    {
        for (uint i = 0; i < cluster.count; ++i)
        {
            uint light_index = light_indices_buffer[cluster.offset + i];

        
            LightData light = light_buffer[light_index];
        
            if (light.type == 2)
                continue;

            float dist = length(light.position - world_pos);
        
        // ライトの範囲外なら無視
            if (dist > light.radius)
                continue;
        
            float3 direction = (light.position - world_pos) / max(dist, 1e-6f);
        
            float attenuation = max(0.0f, 1.0f - (dist / light.radius));
        
        // スポットライトの円錐減衰
            if (light.type == 1)
            {
                float angle = dot(-direction, normalize(light.direction));
                attenuation *= saturate((angle - light.outer_cone) / (light.inner_cone - light.outer_cone));
            }

            radiance += light.color * light.intensity * attenuation;
        }
    }
        
    result[dispatch_index] = float4(radiance, 1.0);

}