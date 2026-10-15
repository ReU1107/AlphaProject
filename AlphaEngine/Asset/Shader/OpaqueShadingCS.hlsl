#include "Resource.hlsli"
#include "Lighting.hlsli"

struct PassConstants
{
    uint result_index;
    uint visibility;
    uint depth;
};

ConstantBuffer<PassConstants> pass_info : register(b2);

float3 DebugColor(uint id1, uint id2)
{
    // MurmurHash3のミキシング（ハッシュ結合）の手法をベースにした実装
    uint h = id1;
    
    // 2つ目のIDを混ぜ込む
    h ^= id2;
    h *= 0xcc9e2d51;
    h = (h << 15) | (h >> 17);
    h = h * 5 + 0xe6546b64;

    // 元のハッシュ関数のファイナライズ処理
    h ^= h >> 16;
    h *= 0x7feb352d;
    h ^= h >> 15;
    h *= 0x846ca68b;
    h ^= h >> 16;

    return float3(
        float((h >> 0) & 0xff) / 255.0,
        float((h >> 8) & 0xff) / 255.0,
        float((h >> 16) & 0xff) / 255.0
    );
}

uint GetUpper16(uint value)
{
    return value >> 16;
}

// 後半（下位）16bitを抽出する関数
uint GetLower16(uint value)
{
    return value & 0xFFFF;
}

struct SurfaceData
{
    // Geometry
    float3 world_position;

    // 最終的にLightingで使用するNormal
    // NormalMap適用後はここを書き換える
    float3 normal;

    // Geometryから復元した元のNormal
    // NormalMapなし、Ray offset、デバッグ等に使える
    float3 geometric_normal;

    float2 uv;

    // Material
    float3 base_color;
    float opacity;

    float metallic;
    float roughness;
    float occlusion;

    float3 emissive;
};
// 1. 法線分布関数 D (GGX)
// 微小面（マイクロファセット）の向きのばらつきを計算します
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / max(denom, 0.000001); // 0除算防止
}

// 2. 幾何関数 G (SmithのSchlick-GGX近似)
// 微小面自身による光の遮蔽（シャドウイング・マスキング）を計算します
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0; // 直接光用の定数k

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx2 * ggx1;
}

// 3. フレネル項 F (Schlickの近似式)
// 視野角（視線と法線の角度）に応じた反射率の変化を計算します
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

// 4. 最終的なCook-Torrance BRDFの計算関数
// 戻り値は「光の反射量」です。メインのライティング計算で「光の色 * NdotL」を掛けて使います
float3 ComputeBRDF(float3 N, float3 V, float3 L, float3 albedo, float metallic, float roughness)
{
    // 各種ベクトルの計算
    float3 H = normalize(V + L); // ハーフベクトル
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    // 物質の基本反射率 F0 の決定（非金属は0.04、金属はアルベドカラー）
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);

    // D, G, F 項の計算
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    // 鏡面反射（スペキュラ）BRDF
    float3 numerator = D * G * F;
    float denominator = 4.0 * NdotV * NdotL;
    float3 specularBRDF = numerator / max(denominator, 0.000001); // 0除算防止

    // エネルギー保存則に基づくディフューズ（拡散反射）の割合計算
    // 金属度が高いほど、光は吸収・浸透せずディフューズは0になります
    float3 kS = F; // 反射された割合
    float3 kD = float3(1.0, 1.0, 1.0) - kS; // 透過・拡散する割合
    kD *= (1.0 - metallic); // 金属ならディフューズを無効化

    // 拡散反射（ランバート）BRDF
    float3 diffuseBRDF = albedo / PI;

    // 最終的なBRDFの合計
    return kD * diffuseBRDF + specularBRDF;
}

float3 DI(SurfaceData surface_data)
{
    StructuredBuffer<LightData> light_buffer = ResourceDescriptorHeap[render_buffers.light];

    float3 V = normalize(render_frame.camera_pos.xyz - surface_data.world_position);

    float3 radiance = float3(0, 0, 0);
    for (uint i = 0; i < render_frame.light_count; ++i)
    {
        LightData light = light_buffer[i];
        
        float3 light_color = light.color * light.intensity;
        float3 direction = -light.direction;
        if (light.type != 2)
        {
            float dist = length(light.position - surface_data.world_position);
        
            // ライトの範囲外なら無視
            if (dist > light.radius)
                continue;
        
            direction = (light.position - surface_data.world_position) / max(dist, 1e-6f);
        
            float attenuation = max(0.0f, 1.0f - (dist / light.radius));
        
            // スポットライトの円錐減衰
            if (light.type == 1)
            {
                float angle = dot(-direction, normalize(light.direction));
                attenuation *= saturate((angle - light.outer_cone) / (light.inner_cone - light.outer_cone));
            }
            light_color *= attenuation;

        }

        
        float3 L = direction;
        float3 N = normalize(surface_data.geometric_normal);
        
        float3 brdf = ComputeBRDF(N, V, L, surface_data.base_color, surface_data.metallic, surface_data.roughness);
        float NdotL = max(dot(N, L), 0.0);

        radiance += brdf * light_color * NdotL;
        
    }
    
    return surface_data.emissive + radiance;
    
}

[numthreads(8, 8, 1)]
void main(uint3 dispatch_index : SV_DispatchThreadID)
{
    RWTexture2D<float4> result = ResourceDescriptorHeap[pass_info.result_index];
    //Texture2D<float4> visibility_tex = ResourceDescriptorHeap[pass_info.visibility];
    Texture2D<uint2> visibility_tex = ResourceDescriptorHeap[pass_info.visibility];
    Texture2D<float> depth_tex = ResourceDescriptorHeap[pass_info.depth];
    
    
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[render_buffers.instance];
    StructuredBuffer<float3> position_buffer = ResourceDescriptorHeap[render_buffers.current_position_srv];
    
    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[render_buffers.mesh];
    
    StructuredBuffer<Meshlet> meshlet_buffer = ResourceDescriptorHeap[render_buffers.meshlet];
    
    StructuredBuffer<uint> vertex_indices_buffer = ResourceDescriptorHeap[render_buffers.vertex_indices];
    StructuredBuffer<uint> triangle_indices_buffer = ResourceDescriptorHeap[render_buffers.triangle_indices];
    
    ByteAddressBuffer attribute_buffer = ResourceDescriptorHeap[render_buffers.attribute];

    StructuredBuffer<MaterialData> material_buffer = ResourceDescriptorHeap[render_buffers.material];
    
    StructuredBuffer<uint> material_table_buffer = ResourceDescriptorHeap[render_buffers.material_table];
    
    StructuredBuffer<MaterialData> override_material_buffer = ResourceDescriptorHeap[render_buffers.override_material];
    
    SamplerState sam = SamplerDescriptorHeap[SAMPLER_PRESET_LINEAR_CLAMP];

    uint2 index = dispatch_index.xy;

    uint2 thread_dimension;
    result.GetDimensions(thread_dimension.x, thread_dimension.y);

    float2 uv = ((float2) dispatch_index + 0.5f) / thread_dimension;
    
    uint2 visibility = visibility_tex.Load(int3(index, 0));
    float depth = depth_tex.Load(int3(index, 0));
    
    uint instance_id = visibility.x;
    
    InstanceData instance = instance_buffer[instance_id];
    
    
    MeshData mesh = mesh_buffer[instance.mesh_index];

    uint local_meshlet_id = GetUpper16(visibility.g);
    uint primitive_id = GetLower16(visibility.g);
    
    uint meshlet_id = mesh.meshlet_element_offset + local_meshlet_id;

    Meshlet meshlet = meshlet_buffer[meshlet_id];

    uint triangle_index_offset = mesh.triangle_element_offset + meshlet.triangle_element_offset;
    
    // meshlet内のプリミティブIDから、元の三角形インデックスを取得
    uint triangle_index = triangle_indices_buffer[triangle_index_offset + primitive_id];
        
    uint idx0 = (triangle_index >> 0) & 0xFFu;
    uint idx1 = (triangle_index >> 8) & 0xFFu;
    uint idx2 = (triangle_index >> 16) & 0xFFu;
    
    uint vertex_index_offset = mesh.vertex_element_offset + meshlet.vertex_element_offset;

    // 元のモデルの頂点インデックスを取得
    uint v0 = vertex_indices_buffer[vertex_index_offset + idx0];
    uint v1 = vertex_indices_buffer[vertex_index_offset + idx1];
    uint v2 = vertex_indices_buffer[vertex_index_offset + idx2];
    
    float3 w0 = mul(instance.world_matrix, float4(position_buffer[instance.vertex_element_offset + v0], 1.0f));
    float3 w1 = mul(instance.world_matrix, float4(position_buffer[instance.vertex_element_offset + v1], 1.0f));
    float3 w2 = mul(instance.world_matrix, float4(position_buffer[instance.vertex_element_offset + v2], 1.0f));
        
    
    float3 world_position = ScreenToWorld(uv, depth, render_frame.inv_view_proj);
    
    float3 barycentrics = ComputeBarycentric(w0, w1, w2, world_position);
    
    
    uint normal_offset = 0;
    uint bone_offset = normal_offset + ((mesh.attribute_flags & VERTEX_ATTR_NORMALS) ? 12 : 0);
    uint uv_offset = bone_offset + ((mesh.attribute_flags & VERTEX_ATTR_BONE_WEIGHTS) ? 32 : 0);

    float3 normal = float3(0.0f, 1.0f, 0.0f);
    float2 mesh_uv = float2(0.0f, 0.0f);
    
    // 重心座標による補間
    float w = barycentrics.x;
    float u = barycentrics.y;
    float v = barycentrics.z;
    
    {
        uint addr0 = mesh.vertex_attribute_byte_offset + (v0 * mesh.vertex_stride) + normal_offset;
        uint addr1 = mesh.vertex_attribute_byte_offset + (v1 * mesh.vertex_stride) + normal_offset;
        uint addr2 = mesh.vertex_attribute_byte_offset + (v2 * mesh.vertex_stride) + normal_offset;
        normal = (asfloat(attribute_buffer.Load3(addr0)) * w) + (asfloat(attribute_buffer.Load3(addr1)) * u) + (asfloat(attribute_buffer.Load3(addr2)) * v);
        
        normal = normalize(normal);
    }
    
    {
        uint addr0 = mesh.vertex_attribute_byte_offset + (v0 * mesh.vertex_stride) + uv_offset;
        uint addr1 = mesh.vertex_attribute_byte_offset + (v1 * mesh.vertex_stride) + uv_offset;
        uint addr2 = mesh.vertex_attribute_byte_offset + (v2 * mesh.vertex_stride) + uv_offset;
        mesh_uv = (asfloat(attribute_buffer.Load2(addr0)) * w) + (asfloat(attribute_buffer.Load2(addr1)) * u) + (asfloat(attribute_buffer.Load2(addr2)) * v);
    }
    
    uint material_index = material_table_buffer[mesh.material_table_index + meshlet.material_index];
    
    MaterialData material = material_buffer[material_index];
    
        
    Texture2D base_tex = ResourceDescriptorHeap[material.base_color.texture_index];

    SamplerState base_sampler = SamplerDescriptorHeap[material.base_color.sampler_index];

    float4 base_color_sample = base_tex.Sample(base_sampler, mesh_uv);
    
    // 
    Texture2D normal_tex = ResourceDescriptorHeap[material.normal.texture_index];

    SamplerState normal_sampler = SamplerDescriptorHeap[material.normal.sampler_index];

    float3 normal_sample = normal_tex.Sample(normal_sampler, mesh_uv).xyz;

    float3 tangent_normal = normal_sample * 2.0 - 1.0;

    // 
    Texture2D metallic_roughness_tex = ResourceDescriptorHeap[material.metallic_roughness.texture_index];

    SamplerState metallic_roughness_sampler = SamplerDescriptorHeap[material.metallic_roughness.sampler_index];

    float4 metallic_roughness_sample = metallic_roughness_tex.Sample(metallic_roughness_sampler, mesh_uv);

    // 
    Texture2D emissive_tex = ResourceDescriptorHeap[material.emissive.texture_index];

    SamplerState emissive_sampler = SamplerDescriptorHeap[material.emissive.sampler_index];

    float3 emissive_sample = emissive_tex.Sample(emissive_sampler, mesh_uv).rgb;


    // Occlusion
    Texture2D occlusion_tex = ResourceDescriptorHeap[material.occlusion.texture_index];

    SamplerState occlusion_sampler = SamplerDescriptorHeap[material.occlusion.sampler_index];

    float occlusion_sample = occlusion_tex.Sample(occlusion_sampler, mesh_uv).r;
    
    material.props;
    
    SurfaceData surface_data;
    surface_data.world_position = world_position;
    surface_data.geometric_normal = normal;
    surface_data.normal = tangent_normal;
    surface_data.emissive = emissive_sample.rgb * material.props.emissive_color * material.props.emissive_intensity;
    surface_data.base_color = base_color_sample.rgb * material.props.base_color;
    surface_data.opacity = base_color_sample.a * material.props.opacity;
    surface_data.metallic = metallic_roughness_sample.r * material.props.metallic;
    surface_data.roughness = metallic_roughness_sample.g * material.props.roughness;
    surface_data.uv = mesh_uv;
  
    
    float3 color = DI(surface_data);
    
    switch (render_frame.debug_value)
    {
        case 0:
            color = DI(surface_data);
            break;
        case 1:
            color = surface_data.emissive;
            break;
        case 2:
            color = surface_data.normal;
            break;
        case 3:
            color = surface_data.geometric_normal;
            break;
        case 4:
            color = DebugColor(visibility.x, visibility.y);
            break;
        case 5:
            color = float3(surface_data.roughness, surface_data.metallic, surface_data.opacity);
            break;
    }
   
    result[index] = float4(color, 1.0f);
}




/*
テスト用visibility_texに色をいれてそのまま表示するだけの
[numthreads(8, 8, 1)]
void main(uint3 dispatch_index : SV_DispatchThreadID)
{
    RWTexture2D<float4> result = ResourceDescriptorHeap[pass_info.result_index];
    Texture2D<float4> visibility_tex = ResourceDescriptorHeap[pass_info.visibility];
    Texture2D<float> depth_tex = ResourceDescriptorHeap[pass_info.depth];
    
    SamplerState sam = SamplerDescriptorHeap[SAMPLER_PRESET_LINEAR_CLAMP];

    uint2 index = dispatch_index.xy;

    uint2 thread_dimension;
    result.GetDimensions(thread_dimension.x, thread_dimension.y);

    float2 uv = ((float2) dispatch_index + 0.5f) / thread_dimension;
    
    float4 visibility = visibility_tex.Sample(sam, uv).rgb;
    
    
    float3 color = visibility_tex.Sample(sam, uv).rgb;

    result[index] = float4(color, 1.0f);
}

*/