#include "Resource.hlsli"
struct VertexOut
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    nointerpolation uint MaterialTableID : MATERIAL_TABLE_ID;
    nointerpolation uint OverrideMaterial : OVERRIDE_MATERIAL;
    nointerpolation uint MaterialID : MATERIAL_ID;

};

float3 DebugColor(uint id)
{
    uint h = id;

    h ^= h >> 16;
    h *= 0x7feb352d;
    h ^= h >> 15;
    h *= 0x846ca68b;
    h ^= h >> 16;

    return float3(
        float((h >> 0) & 0xff) / 255.0,
        float((h >> 8) & 0xff) / 255.0,
        float((h >> 16) & 0xff) / 255.0);
}

float4 main(VertexOut input) : SV_TARGET
{
 
    StructuredBuffer<MaterialData> material_buffer = ResourceDescriptorHeap[render_buffers.material];
    
    StructuredBuffer<uint> material_table_buffer = ResourceDescriptorHeap[render_buffers.material_table];
    
    StructuredBuffer<MaterialData> override_material_buffer = ResourceDescriptorHeap[render_buffers.override_material];
    uint material_index = material_table_buffer[input.MaterialTableID + input.MaterialID];
    
    MaterialData material = material_buffer[material_index];
    
    
    if (input.OverrideMaterial != INVALID_INDEX)
    {
        material = override_material_buffer[input.OverrideMaterial + input.MaterialID];

    }
    
    SamplerState sam = SamplerDescriptorHeap[SAMPLER_PRESET_LINEAR_WRAP];

    Texture2D base_tex = ResourceDescriptorHeap[material.base_color_tex_idx];
    Texture2D emissive_tex = ResourceDescriptorHeap[material.emissive_tex_idx];
    Texture2D normal_tex = ResourceDescriptorHeap[material.normal_tex_idx];

    float3 normal_sample = normal_tex.Sample(sam, input.TexCoord).xyz;
    
    float3 tangent_normal = normal_sample * 2.0 - 1.0;
    
    float4 base_color_sample = base_tex.Sample(sam, input.TexCoord);
    float3 emissive_sample = emissive_tex.Sample(sam, input.TexCoord).rgb;
    
    float4 final_base = float4(material.props.base_color, material.props.opacity) * base_color_sample;
    
    float3 final_emissive = material.props.emissive_color * material.props.emissive_intensity * emissive_sample;
    
    float3 final_rgb = final_base.rgb + final_emissive;
    

    //return float4(final_rgb, final_base.a);
    //return float4(base_color_sample.rgb, final_base.a);
    //return input.Color;
    
    
    switch (render_frame.debug_value)
    {
        case 0:
            return float4(final_rgb, final_base.a);

        case 1:
    // BaseColor
            return float4(base_color_sample.rgb, 1.0);

        case 2:
    // World Normal
            return float4(normalize(input.Normal) * 0.5 + 0.5, 1.0);
        case 3:
            return float4(frac(input.TexCoord), 0.0f, 1.0f);

        case 4:
    // Material ID
            return float4(DebugColor(input.MaterialID), 1.0);

        case 5:
    // MaterialTable ID
            return float4(DebugColor(input.MaterialTableID), 1.0);
        case 6:
            return float4(tangent_normal * 0.5 + 0.5, 1.0);
        case 7:
    // MaterialTable ID
            return float4(DebugColor(material.base_color_tex_idx), 1.0);
        case 8:
            return float4(
        saturate(input.TexCoord.x),
        saturate(input.TexCoord.y),
        0.0,
        1.0);
        case 9:
            {
                float2 uv = input.TexCoord;

                bool negative =
        uv.x < 0.0 || uv.y < 0.0;

                bool over =
        uv.x > 1.0 || uv.y > 1.0;

                if (negative)
                    return float4(1, 0, 0, 1); // 赤: 負

                if (over)
                    return float4(0, 0, 1, 1); // 青: 1超え

                return float4(0, 1, 0, 1); // 緑: 0～1
            }
        default:
            return float4(final_rgb, final_base.a);
    }
}