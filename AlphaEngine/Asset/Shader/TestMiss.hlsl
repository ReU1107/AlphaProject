struct [raypayload] RayPayload
{
    float3 color : write(miss, closesthit) : read(caller);
};

[shader("miss")]
void MyMiss(inout RayPayload payload)
{
    // 背景色（テスト用に青）
    payload.color = float3(0.1, 0.2, 0.7);
}

