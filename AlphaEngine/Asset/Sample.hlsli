
static const float PI = 3.14159265359f;
static const float PI2 = 6.2831853071795864769f;

uint HashUint(uint value)
{
    value = value * 747796405u + 2891336453u;
    value = ((value >> ((value >> 28u) + 4u)) ^ value) * 277803737u;
    return (value >> 22u) ^ value;
}

uint StepHashUint(inout uint seed)
{
    seed = seed * 747796405u + 2891336453u;
    uint word = ((seed >> ((seed >> 28u) + 4u)) ^ seed) * 277803737u;
    return (word >> 22u) ^ word;
}

// [0.0, 1.0) の一様乱数を生成
float Random01(inout uint seed)
{
    uint random_uint = StepHashUint(seed);
    return float(random_uint >> 8u) * (1.0f / 16777216.0f);
}

// 各成分が [0.0, 1.0) の3次元ベクトルを生成
float3 Random3(inout uint seed)
{
    float3 value;
    value.x = Random01(seed);
    value.y = Random01(seed);
    value.z = Random01(seed);
    return value;
}

// [-1.0, 1.0) の一様乱数を生成
float RandomSigned(inout uint seed)
{
    uint random_uint = StepHashUint(seed);
    return float(random_uint >> 8u) * (1.0f / 8388608.0f) - 1.0f;
}

// 各成分が [-1.0, 1.0) の3次元ベクトルを生成
float3 Random3Signed(inout uint seed)
{
    float3 value;
    value.x = RandomSigned(seed);
    value.y = RandomSigned(seed);
    value.z = RandomSigned(seed);
    return value;
}

// 1.0f または -1.0f をランダムに返す
float RandomSign(inout uint seed)
{
    return (StepHashUint(seed) & 1u) != 0u ? 1.0f : -1.0f;
}

// 0 から count - 1 までのランダムなインデックスを返す
uint RandomIndex(inout uint seed, uint count)
{
    return StepHashUint(seed) % count;
}

uint ChooseWeightedAxis(float3 weights, inout uint seed)
{
    float total = weights.x + weights.y + weights.z;
    float value = Random01(seed) * total;
    // 丸めにより total に達する場合も、重み0の末尾候補へ入らない。
    value = min(value, asfloat(asuint(total) - 1u));
    if (value < weights.x)
        return 0u;
    if (value < weights.x + weights.y)
        return 1u;
    return 2u;
}

// 2D単位円の周（ふち）の上のランダムな方向を返す
float2 CircleDirection(inout uint seed)
{
    float angle = PI2 * Random01(seed);
    float sine_value;
    float cosine_value;
    sincos(angle, sine_value, cosine_value);
    return float2(cosine_value, sine_value);
}

float3 SphereDirection(inout uint seed)
{
    float y = Random01(seed) * 2.0f - 1.0f;
    float r = sqrt(max(0.0f, 1.0f - y * y));
    
    float2 xz = CircleDirection(seed);
    
    return float3(xz.x * r, y, xz.y * r);
}

float2 SampleDisk(float radius, inout uint seed)
{
    float radial_distance = radius * sqrt(Random01(seed));
    return radial_distance * CircleDirection(seed);
}

float3 SamplePoint()
{
    return float3(0.0f, 0.0f, 0.0f);
}

/*
円
*/

float3 SampleCircleEdge(float radius, inout uint seed)
{
    float2 xz = CircleDirection(seed) * radius;
    return float3(xz.x, 0.0f, xz.y);
}

float3 SampleCircleSurface(float radius, inout uint seed)
{
    float2 xz = SampleDisk(radius, seed);
    return float3(xz.x, 0.0f, xz.y);
}

/*
Box
*/

float3 SampleBoxVolume(float3 half_size, inout uint seed)
{
    // [-1.0, 1.0]
    return (Random3(seed) * 2.0f - 1.0f) * half_size;
}

float3 SampleBoxVertex(float3 half_size, inout uint seed)
{
    float3 signs;
    signs.x = RandomSign(seed);
    signs.y = RandomSign(seed);
    signs.z = RandomSign(seed);
    return half_size * signs;
}

float3 SampleBoxEdge(float3 half_size, inout uint seed)
{
    // 12辺を長さに比例して選ぶ。同じ軸方向の4辺は等長。
    uint axis = ChooseWeightedAxis(half_size, seed);
    float3 p = SampleBoxVertex(half_size, seed);
    p[axis] = (Random01(seed) * 2.0f - 1.0f) * half_size[axis];
    return p;
}

float3 SampleBoxSurface(float3 half_size, inout uint seed)
{
    // 6面を面積に比例して選ぶ。直方体でも単位面積あたりの密度を揃える。
    float3 areas = float3(half_size.y * half_size.z,
                         half_size.x * half_size.z,
                         half_size.x * half_size.y);
    uint axis = ChooseWeightedAxis(areas, seed);
    float3 p = SampleBoxVolume(half_size, seed);
    p[axis] = half_size[axis] * RandomSign(seed);
    return p;
}

/*
球
*/

float3 SampleSphereVolume(float radius, inout uint seed)
{
    // 中心に偏らないよう、立方根（pow(r, 1/3)）で補正
    float r = radius * pow(Random01(seed), 1.0f / 3.0f);
    return SphereDirection(seed) * r;
}

float3 SampleSphereSurface(float radius, inout uint seed)
{
    return SphereDirection(seed) * radius;
}

/*
半球
*/

float3 SampleHemisphereSurface(float radius, inout uint seed)
{
    float3 p = SampleSphereSurface(radius, seed);
    p.y = abs(p.y);
    return p;
}

float3 SampleHemisphereVolume(float radius, inout uint seed)
{
    float3 p = SampleSphereVolume(radius, seed);
    p.y = abs(p.y);
    return p;
}


/*
円錐
*/
float3 SampleConeVolume(float radius, float height, inout uint seed)
{
    float t = pow(Random01(seed), 1.0f / 3.0f);

    float current_height = t * height;

    float max_radius = radius * t;

    float r = max_radius * sqrt(Random01(seed));
    float2 xz = CircleDirection(seed) * r;

    return float3(xz.x, current_height, xz.y);
}

float3 SampleConeSurface(float radius, float height, inout uint seed)
{
    float slant = sqrt(radius * radius + height * height);
    float choose = Random01(seed) * (slant + radius);
    
    if (choose < slant)
    {
        float t = sqrt(Random01(seed));
        
        float2 xz = CircleDirection(seed) * (radius * t);
        float y = height * t;
        
        return float3(xz.x, y, xz.y);
    }
    else
    {
        float2 xz = SampleDisk(radius, seed);
        return float3(xz.x, height, xz.y);
    }
}

/*
円柱
*/

float3 SampleCylinderVolume(float radius, float height, inout uint seed)
{
    float2 xz = SampleDisk(radius, seed);
    
    float y = (Random01(seed) - 0.5f) * height;
    
    return float3(xz.x, y, xz.y);
}

float3 SampleCylinderSurface(float radius, float height, inout uint seed)
{
    float choose = Random01(seed) * (height + radius);
    
    if (choose < height)
    {
        float2 xz = CircleDirection(seed) * radius;
        float y = (Random01(seed) - 0.5f) * height;
        return float3(xz.x, y, xz.y);
    }
    else
    {
        float2 xz = SampleDisk(radius, seed);
        float y = height * 0.5f * RandomSign(seed);
        return float3(xz.x, y, xz.y);
    }
}

float3 SampleCylinderEdge(float radius, float height, inout uint seed)
{
    float2 xz = CircleDirection(seed) * radius;
    float y = height * 0.5f * RandomSign(seed);
    return float3(xz.x, y, xz.y);
}

/*
矩形
*/

float3 SampleRectangleVertex(float2 half_size, inout uint seed)
{
    float x = half_size.x * RandomSign(seed);
    float z = half_size.y * RandomSign(seed);
    return float3(x, 0.0f, z);
}

float3 SampleRectangleSurface(float2 half_size, inout uint seed)
{
    float x = (Random01(seed) * 2.0f - 1.0f) * half_size.x;
    float z = (Random01(seed) * 2.0f - 1.0f) * half_size.y; 
    return float3(x, 0.0f, z);
}

float3 SampleRectangleEdge(float2 half_size, inout uint seed)
{
    float choose = Random01(seed) * (half_size.x + half_size.y);
    float3 p = SampleRectangleSurface(half_size, seed);
    
    if (choose < half_size.x)
    {
        p.z = half_size.y * RandomSign(seed);
    }
    else
    {
        p.x = half_size.x * RandomSign(seed);
    }
    return p;
}

/*
線分
*/

float3 SampleLineVertex(float line_length, inout uint seed)
{
    float x = line_length * 0.5f * RandomSign(seed);
    return float3(x, 0.0f, 0.0f);
}

float3 SampleLineEdge(float line_length, inout uint seed)
{
    float x = (Random01(seed) - 0.5f) * line_length;
    return float3(x, 0.0f, 0.0f);
}

/*
メッシュ
*/

float3 SampleMeshVertex(StructuredBuffer<float3> position_buffer, StructuredBuffer<uint> index_buffer, uint vertex_offset, uint index_offset, uint index_count, inout uint seed)
{
    // ランダムなインデックスの番号（0 ～ index_count - 1）を取得
    uint random_idx = RandomIndex(seed, index_count);
    
    // インデックスバッファから実際の頂点IDを引く
    uint vertex_id = index_buffer[index_offset + random_idx];
    
    // 頂点座標バッファから座標を取得
    return position_buffer[vertex_offset + vertex_id];
}

// メッシュの面（Surface/三角形のポリゴン内）を一様サンプリングして座標を返す
float3 SampleMeshSurface(StructuredBuffer<float3> position_buffer, StructuredBuffer<uint> index_buffer, uint vertex_offset, uint index_offset, uint index_count, inout uint seed)
{
    uint tri_idx = RandomIndex(seed, index_count / 3) * 3;
    
    // 三角形を構成する3つの頂点IDを取得
    uint v0_id = index_buffer[index_offset + tri_idx + 0];
    uint v1_id = index_buffer[index_offset + tri_idx + 1];
    uint v2_id = index_buffer[index_offset + tri_idx + 2];
    
    // 3頂点のローカル座標を取得
    float3 p0 = position_buffer[vertex_offset + v0_id];
    float3 p1 = position_buffer[vertex_offset + v1_id];
    float3 p2 = position_buffer[vertex_offset + v2_id];

    // 2. 三角形内を一様サンプリング（重心座標系）
    float r1 = Random01(seed);
    float r2 = Random01(seed);
    
    // 対角線の外側にはみ出た場合は、折り返して三角形内に収める補正
    if (r1 + r2 > 1.0f)
    {
        r1 = 1.0f - r1;
        r2 = 1.0f - r2;
    }
    
    // 重み付けして三角形内のランダムな座標を補間計算
    return p0 + r1 * (p1 - p0) + r2 * (p2 - p0);
}

/*
float2 SampleTriangleSurface(float2 a, float2 b, float2 c, inout uint seed);
float2 SampleTriangleEdge(float2 a, float2 b, float2 c, inout uint seed);

float2 SampleRegularPolygonSurface(float radius, uint side_count, inout uint seed);
float2 SampleRegularPolygonEdge(float radius, uint side_count, inout uint seed);

float3 SampleTetrahedronVolume(float3 a, float3 b, float3 c, float3 d, inout uint seed);
float3 SampleTriangle3DSurface(float3 a, float3 b, float3 c, inout uint seed);
float3 SampleSegment(float3 a, float3 b, inout uint seed);
*/