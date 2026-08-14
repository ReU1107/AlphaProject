
static const float PI = 3.14159265359f;

static const float3 RAYLEIGH = float3(5.5e-6f, 13.0e-6f, 22.4e-6f);


// 位相関数(等方)方向に依存しない
float IsotropicPhase()
{
    return 0.25f / PI;
}

// レイリー散乱
float RayleighPhase(float cossita)
{
    return (3.0f / (16.0f * PI)) * (1.0f + cossita * cossita);
}

// ミー散乱
// 位相関数(ヘニエイーグリーンスタイン)
// g[-1,1]
float HGPhase(float cossita, float g)
{
    float g2 = g * g;
    return (1 - g2) / ((4 * PI) * pow((1 + g2 - 2 * g * cossita), 1.5f));
}

// 位相関数(HGの簡易版)
// k[-1,1]
float SchlickPhase(float cossita, float k)
{
    float k2 = k * k;

    return (1 - k2) / ((4 * PI) * pow((1 + k * cossita), 2.0f));
}

float FroxelZToViewDepth(float z, float near, float far)
{
    return near * pow(far / near, z);
}

float ViewDepthToFroxelZ(float depth, float near, float far)
{
    return log(depth / near) / log(far / near);
}

float3 WorldPositionToFroxelUVW(float3 world_pos, float4x4 view, float4x4 view_proj, float near, float far)
{
    float4 clip_pos = mul(view_proj, float4(world_pos, 1.0f));

    float3 ndc_pos = clip_pos.xyz / clip_pos.w;

    float2 uv;
    uv.x = ndc_pos.x * 0.5f + 0.5f;
    uv.y = -ndc_pos.y * 0.5f + 0.5f;


    // Z
    float4 view_pos = mul(view, float4(world_pos, 1.0f));

    float view_depth = view_pos.z;

    float z = ViewDepthToFroxelZ(view_depth, near, far);

    return float3(uv, z);
}