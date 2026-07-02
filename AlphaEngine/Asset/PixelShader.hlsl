struct VertexOut
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

float4 main(VertexOut input) : SV_TARGET
{
    return input.Color;
}