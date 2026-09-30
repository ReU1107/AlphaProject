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
    
    Texture2D base_tex = ResourceDescriptorHeap[material.base_color.texture_index];

    SamplerState base_sampler = SamplerDescriptorHeap[material.base_color.sampler_index];

    float4 base_color_sample = base_tex.Sample(base_sampler, input.TexCoord);
    
    // 
    Texture2D normal_tex = ResourceDescriptorHeap[material.normal.texture_index];

    SamplerState normal_sampler = SamplerDescriptorHeap[material.normal.sampler_index];

    float3 normal_sample = normal_tex.Sample(normal_sampler, input.TexCoord).xyz;

    float3 tangent_normal = normal_sample * 2.0 - 1.0;

    // 
    Texture2D metallic_roughness_tex = ResourceDescriptorHeap[material.metallic_roughness.texture_index];

    SamplerState metallic_roughness_sampler = SamplerDescriptorHeap[material.metallic_roughness.sampler_index];

    float4 metallic_roughness_sample = metallic_roughness_tex.Sample( metallic_roughness_sampler, input.TexCoord);

    // 
    Texture2D emissive_tex = ResourceDescriptorHeap[material.emissive.texture_index];

    SamplerState emissive_sampler = SamplerDescriptorHeap[material.emissive.sampler_index];

    float3 emissive_sample = emissive_tex.Sample( emissive_sampler, input.TexCoord).rgb;


    // Occlusion
    Texture2D occlusion_tex = ResourceDescriptorHeap[material.occlusion.texture_index];

    SamplerState occlusion_sampler = SamplerDescriptorHeap[material.occlusion.sampler_index];

    float occlusion_sample = occlusion_tex.Sample( occlusion_sampler, input.TexCoord).r;


    // Material properties
    float4 final_base = float4( material.props.base_color, material.props.opacity)  * base_color_sample;

    float3 final_emissive =
    material.props.emissive_color
    * material.props.emissive_intensity
    * emissive_sample;

    float3 final_rgb = final_base.rgb + final_emissive;
    

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
            return float4(DebugColor(material.base_color.texture_index), 1.0);
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