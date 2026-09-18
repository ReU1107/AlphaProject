struct VertexOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

float4 main(in VertexOut input) : SV_TARGET
{
    float2 center_uv = input.uv - 0.5;

    float dist = length(center_uv);

    float alpha = saturate(1.0 - (dist / 0.5));
    
    return float4(input.color.rgb, alpha);
}