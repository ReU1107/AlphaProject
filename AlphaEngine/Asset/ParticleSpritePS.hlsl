struct VertexOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

float4 main(in VertexOut input) : SV_TARGET
{
    return float4(input.color.rgb, 1.0f);
}