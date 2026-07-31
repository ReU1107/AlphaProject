#include "Resource.hlsli"

// 空のPayload（テスト用）
struct [raypayload] RayPayload
{
    float3 color : write(miss, closesthit) : read(caller);
};

ConstantBuffer<SystemBufferIndices> buffer_indices : register(b0);
ConstantBuffer<FrameRenderContext> frame : register(b1);

[shader("raygeneration")]
void MyRayGen()
{
    // ピクセル中心座標をクリップ空間座標に変換
    uint2 index = DispatchRaysIndex().xy;
    // 1. ピクセル中心を 0.0 ～ 1.0 の範囲に正規化 (UV空間)
    // ※インデックスのままだと左上すぎるので、+0.5 してピクセルの中央を狙うのがプロの鉄則です
    float2 uv = ((float2) index + 0.5f) / float2(DispatchRaysDimensions().xy);
    
    // 2. クリップ空間 (NDC) 座標に変換 (X: -1～1, Y: 1～-1)
    float2 clipSpacePos = uv * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);

    // 3. クリップ空間の手前（ニアプレーン Z=0.0）の点をワールド空間に逆変換
    // ※お使いのエンジンの行列の向きに合わせて mul の順序を調整してください
    // 一般的な HLSL (Row-Major) の場合はベクトルが先です： mul(float4, matrix)
    float4 targetWorldPos = mul(frame.proj_view, float4(clipSpacePos, 0.0f, 1.0f));
    targetWorldPos.xyz /= targetWorldPos.w; // 透視除算して正しいワールド座標へ

    // 4. カメラ位置（レイの始点）と、ターゲット位置から方向ベクトルを計算
    float3 origin = frame.camera_pos.xyz;
    float3 direction = normalize(targetWorldPos.xyz - origin);

    RayDesc ray;
    ray.Origin = origin;
    ray.Direction = direction;
    ray.TMin = 0.001;
    ray.TMax = 1000.0;

    RayPayload payload;
    payload.color = float3(0, 0, 0);

    RaytracingAccelerationStructure tlas = ResourceDescriptorHeap[buffer_indices.rt_structure];
    RWTexture2D<float4> outputTexture = ResourceDescriptorHeap[buffer_indices.rt_result];
    
    // レイを飛ばすテスト
    TraceRay(tlas, 0, 0xFF, 0, 1, 0, ray, payload);
    
    outputTexture[index.xy] = float4(payload.color, 1.0f);

}
