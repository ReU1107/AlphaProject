struct LineVertex
{
    float3 position : POSITION;
    float4 color    : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct LineConstants
{
    float4x4 view_projection;
};

ConstantBuffer<LineConstants> constants : register(b0);


VSOutput main(in LineVertex input)
{
    float4 position = mul(constants.view_projection,float4(input.position, 1.0f));
    
    VSOutput output;
    output.position = position;
    output.color = input.color;
    
    return output;
}