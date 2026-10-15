
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

float3 ScreenToWorld(float2 uv, float depth, float4x4 inv_view_proj)
{

    float2 clip_space_pos = uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 ndc_pos = float4(clip_space_pos, depth, 1.0f);
    
    float4 world_pos = mul(inv_view_proj, ndc_pos);

    return world_pos.xyz / world_pos.w;
}

float3 ComputeBarycentric(float3 A, float3 B, float3 C, float3 P)
{
    float3 v0 = B - A;
    float3 v1 = C - A;
    float3 v2 = P - A;

    // 各三角形の法線ベクトル（外積）の大きさを利用して面積比を出す
    float3 n = cross(v0, v1);
    float denom = dot(n, n);

    float edge_scale = max(dot(v0, v0), dot(v1, v1));

    float epsilon = edge_scale * edge_scale * 1e-8f;

    if (denom < epsilon)
    {
        return float3(1.0f, 0.0f, 0.0f);
    }

    float3 nC = cross(v0, v2);
    float3 nB = cross(v2, v1);

    // 面積比から重心座標を計算
    float u = dot(n, nB) / denom;
    float v = dot(n, nC) / denom;
    float w = 1.0f - u - v;

    return float3(w, u, v);
}