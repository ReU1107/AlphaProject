
SamplerState sam : register(s0);
Texture2D tex : register(t0);

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;

};


float4 main(in PSInput input) : SV_TARGET
{
    //return float4(1.0f, 0.0f, 0.0f, 0.5f);
    //return input.color;
    
    float4 color = input.color * tex.Sample(sam, input.texcoord);
    return color;
}