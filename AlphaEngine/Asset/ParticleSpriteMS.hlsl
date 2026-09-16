#include "Resource.hlsli"

#include "Particle.hlsli"
struct VertexOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

ConstantBuffer<FrameRenderConstants> frame_constants : register(b1);
ConstantBuffer<ParticleBufferIndices> buffer_indices : register(b2);

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
    
    StructuredBuffer<ParticleData> particle_buffer = ResourceDescriptorHeap[buffer_indices.render_particle];
    ParticleData particle =  particle_buffer[particle_index];

    SetMeshOutputCounts(4, 2);

    float3 center = particle.position;
    const float half_size = 0.15f;

    float3 right = frame_constants.camera_right * particle.size;

    float3 up = frame_constants.camera_up.xyz * particle.size;

    float3 p0 = center - right + up;
    float3 p1 = center + right + up;
    float3 p2 = center - right - up;
    float3 p3 = center + right - up;
    
    float4 color = unpackUnorm4x8(particle.packed_color);

    vertices[0].position = mul(frame_constants.view_proj, float4(p0, 1.0f));
    vertices[1].position = mul(frame_constants.view_proj, float4(p1, 1.0f));
    vertices[2].position = mul(frame_constants.view_proj, float4(p2, 1.0f));
    vertices[3].position = mul(frame_constants.view_proj, float4(p3, 1.0f));

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