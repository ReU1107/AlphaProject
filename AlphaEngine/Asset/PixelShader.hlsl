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

// マテリアルの基本物理パラメータ（合計64バイト / 16バイト境界アライメント済み）
struct MaterialProperties
{
    float3 base_color; // 12バイト - 基本色
    float opacity; //  4バイト - 不透明度 (16バイト境界)

    float metallic; //  4バイト - 金属感
    float roughness; //  4バイト - 粗さ
    float emissive_intensity; //  4バイト - 自発光の強さ
    float occlusion; //  4バイト - アンビエントオクルージョン (32バイト境界)

    float3 emissive_color; // 12バイト - 自発光の色
    float refraction_index; //  4バイト - 屈折率 (48バイト境界)

    float3 specular_tint; // 12バイト - 鏡面反射の微調整用
    float padding; //  4バイト - 16バイト境界合わせ用のパディング (64バイト境界)
};

// マテリアル全体のデータ（合計96バイト）
struct MaterialData
{
    MaterialProperties props; // 64バイト - 上記のプロパティ実体

    uint base_color_tex_idx; //  4バイト - テクスチャバインドレスインデックス (0xFFFFFFFF = 無効)
    uint normal_tex_idx; //  4バイト - 法線マップインデックス
    uint metallic_roughness_tex_idx; // 4バイト
    uint emissive_tex_idx; //  4バイト

    uint occlusion_tex_idx; //  4バイト
    uint toon_tex_idx; //  4バイト - MMD等のトゥーンテクスチャ用
    uint sphere_tex_idx; //  4バイト - MMD等のスフィアマップ用
    uint flags; //  4バイト - マテリアル判別フラグ
};

struct HeapIndices
{
    float4x4 view_proj;
    uint position;
    uint attribute;
    uint vertex_indices;
    uint triangle_indices;
    uint meshlet;
    uint mesh;
    uint material;
    uint material_table;
};
ConstantBuffer<HeapIndices> heap_indices : register(b1);


float4 main(VertexOut input) : SV_TARGET
{
 
    StructuredBuffer<MaterialData> material_buffer = ResourceDescriptorHeap[heap_indices.material];
    
    ByteAddressBuffer material_table_buffer = ResourceDescriptorHeap[heap_indices.material_table];
    
    uint material_offset = material_table_buffer.Load(input.MaterialTableID);
    
    MaterialData material = material_buffer[material_offset + input.MaterialID];
        
    Texture2D tex = ResourceDescriptorHeap[material.base_color_tex_idx];

    return tex.Sample(sam, input.TexCoord);    
}