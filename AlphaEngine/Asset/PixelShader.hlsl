struct VertexOut
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    nointerpolation uint MaterialID : MATERIAL_ID;

};

SamplerState sam : register(s0);
struct GPUMaterialData
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
    
    uint base_color_tex_idx;
    uint normal_tex_idx;
    uint metallic_roughness_tex_idx;
    uint emissive_tex_idx;
    uint occlusion_tex_idx;
    uint toon_tex_idx;
    uint sphere_tex_idx;
    uint flags;
};



struct TestConstants
{
    uint MaterialHeapIndex; //
    uint3 padding;
};

ConstantBuffer<TestConstants> cb : register(b1);


float4 main(VertexOut input) : SV_TARGET
{
    
    //Texture2D tex = ResourceDescriptorHeap[8];

    //return tex.Sample(sam, input.TexCoord);
    
    StructuredBuffer<GPUMaterialData> materialBuffer = ResourceDescriptorHeap[cb.MaterialHeapIndex];
    GPUMaterialData material = materialBuffer[input.MaterialID];
    Texture2D tex = ResourceDescriptorHeap[NonUniformResourceIndex(material.base_color_tex_idx)];
    //Texture2D tex = ResourceDescriptorHeap[8];
    
    return tex.Sample(sam, input.TexCoord);    
    //return float4(
    //frac(input.MaterialID / 8.0),
    //frac(input.MaterialID / 16.0),
    //frac(input.MaterialID / 32.0),
    //1.0);
    
    //return float4(input.TexCoord, 0.0f, 1.0f);
    
}