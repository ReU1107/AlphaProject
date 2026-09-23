#include "Resource.hlsli"
#include "Lighting.hlsli"

// 空のPayload（テスト用）
struct [raypayload] RayPayload
{
    float visibility : write(miss, closesthit) : read(caller);
};

struct PassConstants
{
    uint volume;
    uint depth;
    uint radiance;
    uint transmittance;
};

// https://www.pbr-book.org/4ed/Volume_Scattering/Volume_Scattering_Processes
// キルヒホフの放射法則
ConstantBuffer<PassConstants> pass_info : register(b2);

float PseudoRandom(uint2 p)
{
    return frac(sin(dot(float2(p), float2(12.9898, 78.233))) * 43758.5453);
}

float InterleavedGradientNoise(float2 pixel_coord)
{
    float3 magic = float3(0.06711056, 0.00583715, 52.9829189);
    return frac(magic.z * frac(dot(pixel_coord, magic.xy)));
}

[shader("raygeneration")]
void RayGenerate()
{   
    
    RaytracingAccelerationStructure tlas = ResourceDescriptorHeap[render_buffers.rt_structure];
    StructuredBuffer<LightData> light_buffer = ResourceDescriptorHeap[render_buffers.light];

    Texture3D<float4> volume_tex = ResourceDescriptorHeap[pass_info.volume];
    Texture2D<float> depth_tex = ResourceDescriptorHeap[pass_info.depth];
    SamplerState sam = SamplerDescriptorHeap[SAMPLER_PRESET_LINEAR_CLAMP];
    
    RWTexture2D<float4> radiance_tex = ResourceDescriptorHeap[pass_info.radiance];
    RWTexture2D<float4> transmittance_tex = ResourceDescriptorHeap[pass_info.transmittance];

    uint2 index = DispatchRaysIndex().xy;

    float2 uv = ((float2) index + 0.5f) / float2(DispatchRaysDimensions().xy);
    
    float2 ndc_pos = uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);

    float4 far_clip_pos = mul(render_frame.inv_view_proj, float4(ndc_pos, 0.0f, 1.0f));
    far_clip_pos.xyz /= far_clip_pos.w;
    
    float3 origin = render_frame.camera_pos.xyz;
    float3 direction = normalize(far_clip_pos.xyz - origin);
    
    float depth = depth_tex.SampleLevel(sam, uv, 0).r;
    
    float4 geometry_world_pos = mul(render_frame.inv_view_proj, float4(ndc_pos, depth, 1.0f));
    geometry_world_pos.xyz /= geometry_world_pos.w;
    
    // カメラから遮蔽物（不透明オブジェクト）までの距離（最大距離）
    float max_t = length(geometry_world_pos.xyz - origin);
    float jitter = InterleavedGradientNoise(uv);
    
    uint step_count = 100;
    float step_size = 1.0f;

    float3 radiance = 0.0;          // 深度値までの空間の色
    float3 transmittance = 1.0;     // 深度値までで残った透過度
    
    LightData light = light_buffer[0];  // とりあえず固定値でしてるけど今後directional lightのindexか一番影響の高いライト番号を渡す
    
    float3 light_dir = -light.direction;

    float cossita = dot(direction, light_dir);
    float3 phase_r = RayleighPhase(cossita);
    float3 fog_color = float3(1.0f, 1.0f, 1.0f);
    float3 phase_m = HGPhase(cossita, 0.75f); // gの部分は今後場所によって変わるように

    for (uint i = 0; i < step_count; ++i)
    {
        float t = (i + jitter) * step_size;
        if (t >= max_t)
        {
            break;
        }
        
        float3 position = origin + (direction * t);

        
        RayDesc ray;
        ray.Origin = position + light_dir * 0.001f;
        ray.Direction = light_dir;
        ray.TMin = 0.001;
        ray.TMax = 1000.0;

        RayPayload payload;
        payload.visibility = 1.0f;
    
        TraceRay(tlas, 0, 0xFF, 0, 0, 0, ray, payload);
        
        float3 uvw = WorldPositionToFroxelUVW(position, render_frame.view, render_frame.view_proj, 0.1f, 1000.0f);
        
        float3 volume_radiance = volume_tex.SampleLevel(sam, uvw, 0).xyz;

        
        float height = position.y;

        float density_r = exp(-max(height, 0.0f) / 8000.0f);

        float density_m = 0.03f; // ここも特性に応じて変化

        
        float3 sigma_r = RAYLEIGH * density_r;
        float3 sigma_m = fog_color * density_m * 2.0f; // 
        float3 sigma_a = float3(0.0f, 0.0f, 0.0f); // 吸収係数(一旦なし)
        
        float3 sigma_s = sigma_r + sigma_m;
        float3 sigma_t = sigma_s + sigma_a;
        
        // 等方性として処理
        radiance += transmittance * volume_radiance * IsotropicPhase() * step_size * sigma_s;
        
        float3 scattering = sigma_r * phase_r + sigma_m * phase_m;
        float3 dominant = light.color * light.intensity * payload.visibility;
        
        // メインライト(DirectionalLight)だけ
        radiance += transmittance * dominant * scattering * step_size;
        
        
        // ランベルトベールで減衰
        transmittance *= exp(-sigma_t * step_size);
    }

    
    
    radiance_tex[index] = float4(radiance, 1.0f);
    transmittance_tex[index] = float4(transmittance, 1.0f);

    //radiance_tex[index] = float4(1.0f, 0.0f, 0.0f, 1.0f);
    //transmittance_tex[index] = float4(0.0f, 1.0f, 0.0f, 1.0f);
    
}



[shader("closesthit")]
void OpaqueHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    payload.visibility = 0.0f;
}

[shader("miss")]
void Miss(inout RayPayload payload)
{
    payload.visibility = 1.0f;
}

// 一旦シャドウオンリーにしとく

//[shader("closesthit")]
//void TranslucentHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
//{
    
    
//    // 半透明はとりあえず保留
//    // 一番困るねww色だけつけてignorehit判定でもする?w
    
//}


//struct VolumeAABBAttributes
//{
//    float t_entry; // ボリュームに入った距離
//    float t_exit; // ボリュームから出た距離
//};


//[shader("closesthit")]
//void VolumeHit(inout RayPayload payload, in VolumeAABBAttributes attr)
//{
//   // StructuredBuffer<VolumeData> volume_buffer = ResourceDescriptorHeap[render_buffers.volume];

//    // 3dテクスチャの形状は今後ね....
    
    
//    // レイマーチを行いながら陰影判定飛ばすレイで他のvolumeをどうするかだな
//    // 雲とかの判定できるかもしれん計算負荷はないものとするwww
    
    
    
    
    
//    //payload.color = float3(0.0f, 1.0f, 0.0f);
    
//}

//[shader("closesthit")]
//void OpaqueShadowHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
//{
//    // 不透明なオブジェクトに当たったらそのまま終了する
    
//}


//[shader("closesthit")]
//void TranslucentShadowHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
//{
    
    
//}


//[shader("closesthit")]
//void VolumeShadowHit(inout RayPayload payload, in VolumeAABBAttributes attr)
//{
    
//}

//[shader("intersection")]
//void VolumeIntersection()
//{
//    float3 ray_origin = ObjectRayOrigin();
//    float3 ray_direction = ObjectRayDirection();
//    float ray_t_min = RayTMin();

//    float3 inv_ray_dir = 1.0f / ray_direction;

//    StructuredBuffer<VolumeData> volume_buffer = ResourceDescriptorHeap[render_buffers.volume];

//    uint index = InstanceID();
    
//    VolumeData volume = volume_buffer[index];
    
//    float3 min_position = volume.min;
//    float3 max_position = volume.max;
    
//    // (min - o) / d, (max - o) / d
//    float3 t0 = (min_position - ray_origin) * inv_ray_dir;
//    float3 t1 = (max_position - ray_origin) * inv_ray_dir;

//    // レイの方向が負の場合、t0とt1の大小関係が逆転するので並び替える
//    float3 t_min_axes = min(t0, t1);
//    float3 t_max_axes = max(t0, t1);

//    // すべての軸で共通して重なっている「入り口」と「出口」の距離を求める
//    float t_entry = max(max(t_min_axes.x, t_min_axes.y), max(t_min_axes.z, RayTMin()));
//    float t_exit = min(min(t_max_axes.x, t_max_axes.y), min(t_max_axes.z, RayTCurrent()));

//    // 有効な交差がある条件：入り口が出口より手前にあり、かつ前方に存在すること
//    if (t_entry < t_exit && t_exit > 0.0f)
//    {
//        VolumeAABBAttributes attr;
//        attr.t_entry = t_entry;
//        attr.t_exit = t_exit;

//        ReportHit(t_entry, 0, attr);
//    }
//}