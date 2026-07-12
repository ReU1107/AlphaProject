struct VSInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
};
struct PSInput
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
};

struct MeshConstants
{
    float4x4 WorldViewProj;
    uint VertexHeapIndex; // 元の頂点データのバッファ
    uint VertexAttributeHeapIndex; // 元の頂点属性データのバッファ
    uint MeshletMetaHeapIndex; // global_meshlets バッファ
    uint MeshletVertHeapIndex; // global_meshlet_vertices バッファ
    uint MeshletTriHeapIndex; // global_meshlet_triangles バッファ
    uint MaterialHeapIndex; // \
    
    // どこかに移動させるかも
    uint VertexStride; // 
    uint AttributeFlags; // 
    
};
ConstantBuffer<MeshConstants> cb : register(b0);

PSInput main(VSInput input)
{
    PSInput output = (PSInput)0;
    output.Position = mul(cb.WorldViewProj, float4(input.Position, 1.0f));
    output.TexCoord = input.TexCoord;
    return output;
}