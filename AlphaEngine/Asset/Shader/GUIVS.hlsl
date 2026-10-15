struct GUIVertex
{
    float2 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 texcoord : TEXCOORD;

};

struct GUIConstants
{
    float4x4 projection;
};

ConstantBuffer<GUIConstants> constants : register(b0);


VSOutput main(in GUIVertex input)
{
    float4 position = mul(constants.projection, float4(input.position, 0.0f, 1.0f));
    
    VSOutput output;
    output.position = position;
    output.color = input.color;
    output.texcoord = input.texcoord;
    
    return output;
}