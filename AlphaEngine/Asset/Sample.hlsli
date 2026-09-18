
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

// 2D単位円の周（ふち）の上のランダムな方向を返す
float2 CircleDirection(inout uint seed)
{
    float angle = PI2 * Random01(seed);
    float sine_value;
    float cosine_value;
    sincos(angle, sine_value, cosine_value);
    return float2(cosine_value, sine_value);
}

// 2D円盤（ディスク）の内側を一様サンプリング
float2 SampleDisk(float radius, inout uint seed)
{
    float radial_distance = radius * sqrt(Random01(seed));
    return radial_distance * CircleDirection(seed);
}

// --- 1. Box（立方体・直方体） ---
// extents: 各軸の幅・高さ・奥行き（サイズ）
float3 SampleBoxVolume(float3 extents, inout uint seed)
{
    // [-0.5, 0.5] の範囲に散らして extents を掛ける
    return (Random3(seed) - 0.5f) * extents;
}

float3 SampleBoxSurface(float3 extents, inout uint seed)
{
    float3 r = Random3(seed);
    uint face = RandomIndex(seed, 6);
    float3 p = (r - 0.5f) * extents;
    
    // 6面のうち選ばれた1面を外枠に固定する
    if (face == 0)
        p.x = extents.x * 0.5f;
    if (face == 1)
        p.x = -extents.x * 0.5f;
    if (face == 2)
        p.y = extents.y * 0.5f;
    if (face == 3)
        p.y = -extents.y * 0.5f;
    if (face == 4)
        p.z = extents.z * 0.5f;
    if (face == 5)
        p.z = -extents.z * 0.5f;
    
    return p;
}

// --- 2. Sphere（球体） ---
// 3D単位球面上のランダムな方向（法線としても使える）
float3 SphereDirection(inout uint seed)
{
    float u = Random01(seed);
    float v = Random01(seed);
    float theta = u * PI2;
    float phi = acos(2.0f * v - 1.0f);
    
    float sin_phi, cos_phi;
    sincos(phi, sin_phi, cos_phi);
    float sin_theta, cos_theta;
    sincos(theta, sin_theta, cos_theta);
    
    return float3(sin_phi * cos_theta, sin_phi * sin_theta, cos_phi);
}

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

// --- 3. Cone（円錐） ---
// angle_rad: 開口角（ラジアン）。0〜PI/2 で指定
// height: 円錐の高さ（Z軸方向を想定）
float3 SampleConeVolume(float angle_rad, float height, inout uint seed)
{
    float h = Random01(seed); // 根元から先端までの高さ割合
    float current_height = h * height;
    
    // 高さに応じた底面の最大半径
    float max_radius = current_height * tan(angle_rad);
    // 円盤内を一様サンプリング
    float2 disk = SampleDisk(max_radius, seed);
    
    return float3(disk.x, disk.y, current_height);
}

// --- 4. Cylinder（円柱） ---
// radius: 円柱の半径、height: 円柱の高さ（Z軸方向を想定）
float3 SampleCylinderVolume(float radius, float height, inout uint seed)
{
    float2 disk = SampleDisk(radius, seed);
    float z = (Random01(seed) - 0.5f) * height; // 中心を基準に上下に広げる
    return float3(disk.x, disk.y, z);
}

float3 SampleCylinderSurface(float radius, float height, inout uint seed)
{
    float u = Random01(seed);
    float total_area = (PI2 * radius * height) + (2.0f * PI * radius * radius);
    float side_area = PI2 * radius * height;
    
    // 面積比に応じて、側面か「底面・天面」かをランダムに選択
    if (u * total_area < side_area)
    {
        // 側面
        float2 dir = CircleDirection(seed) * radius;
        float z = (Random01(seed) - 0.5f) * height;
        return float3(dir.x, dir.y, z);
    }
    else
    {
        // 上下の蓋（天面か底面か）
        float2 disk = SampleDisk(radius, seed);
        float z = height * 0.5f * RandomSign(seed);
        return float3(disk.x, disk.y, z);
    }
}

// メッシュの頂点（Vertex）からランダムに1つ選んで座標を返す
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