#include "Particle.hlsli"

RWStructuredBuffer<Particle> particles : register(u0);
RWByteAddressBuffer dead_list : register(u1);

ConstantBuffer<EmitInfo> info : register(b0);

struct Sphere
{
    float3 center; // 中心座標
    float radius; // 半径
};

ConstantBuffer<Sphere> shape : register(b1);

static const float PI = 3.14159265359f;

// ランダムな値から球上のベクトル取得単位方向なので方向として扱う
float3 SamplingSphere(float2 texcoord)
{
    float y = 2.0 * texcoord.x - 1;
    float theta = 2.0 * PI * texcoord.y;
    float r = sqrt(max(0.0f, 1.0 - y * y));
    return float3(
        cos(theta) * r,
        y,
        sin(theta) * r
    );
}

float2 Hash21(uint seed)
{
    uint n = seed * 1103515245 + 12345;
    uint x = (n ^ (n >> 3)) * 1103515245;
    uint y = (x ^ (x >> 16)) * 1103515245;
    return float2(float(x) / 4294967295.0, float(y) / 4294967295.0);
}

[numthreads(64, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    
    if (DTid.x >= info.max_num)
        return;

    int index = 0;
    dead_list.InterlockedAdd(OFFSET_COUNT, -1, index);
    
    if (index <= 0)
    {
        dead_list.InterlockedAdd(OFFSET_COUNT, 1, index);
        return;
    }

    uint fetch_pos = (uint)index - 1;
    uint free_index = dead_list.Load(OFFSET_DATA_START + ELEMENT_SIZE * fetch_pos);
    
    float2 texcoord = Hash21(DTid.x);
    float3 dir = SamplingSphere(texcoord);
    
    float3 spawn_pos = shape.center + (shape.radius * dir);
    
    Particle particle = (Particle) 0;
    particle.position = spawn_pos;
    particle.life_time = info.life_time;
    particle.direction = dir;
    particle.max_life_time = info.life_time;
    particle.color = info.color;
    
    particles[free_index] = particle;
}