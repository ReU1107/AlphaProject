struct VertexOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

float4 main() : SV_TARGET
{
	return float4(0.0f, 1.0f, 1.0f, 1.0f);
}