#include "Resource.hlsli"
struct VertexOut
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    nointerpolation uint MaterialTableID : MATERIAL_TABLE_ID;
    nointerpolation uint MaterialID : MATERIAL_ID;

};

SamplerState sam : register(s0);


ConstantBuffer<SystemBufferIndices> buffer_indices : register(b0);


float4 main(VertexOut input) : SV_TARGET
{
 
    StructuredBuffer<MaterialData> material_buffer = ResourceDescriptorHeap[buffer_indices.material];
    
    ByteAddressBuffer material_table_buffer = ResourceDescriptorHeap[buffer_indices.material_table];
    
    uint material_offset = material_table_buffer.Load(input.MaterialTableID);
    
    MaterialData material = material_buffer[material_offset + input.MaterialID];
        
    Texture2D base_tex = ResourceDescriptorHeap[material.base_color_tex_idx];
    Texture2D emissive_tex = ResourceDescriptorHeap[material.emissive_tex_idx];

    float4 base_color_sample = base_tex.Sample(sam, input.TexCoord);
    float3 emissive_sample = emissive_tex.Sample(sam, input.TexCoord).rgb;
    
    // ベースカラー = 定数基本色 * テクスチャカラー、 アルファ = 定数不透明度 * テクスチャアルファ
    float4 final_base = float4(material.props.base_color, material.props.opacity) * base_color_sample;
    
    // エミッシブ = 定数自発光色 * 定数強度 * テクスチャカラー
    float3 final_emissive = material.props.emissive_color * material.props.emissive_intensity * emissive_sample;
    
    // ベースカラーに自発光成分を加算
    float3 final_rgb = final_base.rgb + final_emissive;
    
    return float4(final_rgb, final_base.a);
}