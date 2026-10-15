#include "Resource.hlsli"

#include "Particle.hlsli"
struct VertexOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

float4 unpackUnorm4x8(uint packedVal)
{
    uint4 u;
    u.x = packedVal & 0xFFu;
    u.y = (packedVal >> 8) & 0xFFu;
    u.z = (packedVal >> 16) & 0xFFu;
    u.w = (packedVal >> 24) & 0xFFu;

    return float4(u) / 255.0;
}

[outputtopology("triangle")]
[numthreads(1, 1, 1)]
void main(
    uint3 group_id : SV_GroupID,
    out vertices VertexOut vertices[4],
    out indices uint3 triangles[2])
{
    uint particle_index = group_id.x;
    
    StructuredBuffer<ParticleData> particle_buffer = ResourceDescriptorHeap[particle_buffers.render_particle];
    ParticleData particle =  particle_buffer[particle_index];

    SetMeshOutputCounts(4, 2);

    float3 center = particle.position;
    
    float t = saturate(particle.age / particle.lifetime);
    
    float3 start_size = particle.start_size;
    float3 end_size = particle.end_size;
   
    float3 size = lerp(start_size, end_size, t);
    
    float3 right = render_frame.camera_right * size;

    float3 up = render_frame.camera_up.xyz * size;

    float3 p0 = center - right + up;
    float3 p1 = center + right + up;
    float3 p2 = center - right - up;
    float3 p3 = center + right - up;
    
    float4 start_color = unpackUnorm4x8(particle.packed_start_color);
    float4 end_color = unpackUnorm4x8(particle.packed_end_color);
    
    float4 color = lerp(start_color, end_color, t);

    vertices[0].position = mul(render_frame.view_proj, float4(p0, 1.0f));
    vertices[1].position = mul(render_frame.view_proj, float4(p1, 1.0f));
    vertices[2].position = mul(render_frame.view_proj, float4(p2, 1.0f));
    vertices[3].position = mul(render_frame.view_proj, float4(p3, 1.0f));

    vertices[0].uv = float2(0, 0);
    vertices[1].uv = float2(1, 0);
    vertices[2].uv = float2(0, 1);
    vertices[3].uv = float2(1, 1);
    
    vertices[0].color = color;
    vertices[1].color = color;
    vertices[2].color = color;
    vertices[3].color = color;

    triangles[0] = uint3(0, 1, 2);
    triangles[1] = uint3(2, 1, 3);
}