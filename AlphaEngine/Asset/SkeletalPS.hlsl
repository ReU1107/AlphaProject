Texture2D albedo : register(t1);

struct MaterialData
{
    float3 base_color;
    float opacity;

    float metallic;
    float roughness;
    float emissive_intensity;
    float occlusion;

    float3 emissive_color;
    float refraction_index;

    float3 specular_tint;
    float padding;
};


ConstantBuffer<MaterialData> Material : register(b1);

SamplerState sam : register(s0);
struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAl;
    float2 texcoord : TEXCOORD;
};

float4 main(in PSInput input) : SV_TARGET
{
    float3 light_dir = normalize(float3(-1, -1, -1));
    
    float4 color = albedo.Sample(sam, input.texcoord);
    color.a = 1.0f;
    
    return color;
}