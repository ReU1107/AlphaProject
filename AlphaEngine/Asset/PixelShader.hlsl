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
        
    Texture2D tex = ResourceDescriptorHeap[material.base_color_tex_idx];

    return tex.Sample(sam, input.TexCoord);    
}