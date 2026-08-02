#include "Resource.hlsli"

struct [raypayload] RayPayload
{
    float3 color : write(miss, closesthit) : read(caller);
};

SamplerState sam : register(s0);

ConstantBuffer<SystemBufferIndices> buffer_indices : register(b0);



[shader("closesthit")]
void MyClosestHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    StructuredBuffer<InstanceData> instance_buffer  = ResourceDescriptorHeap[buffer_indices.instance];
    StructuredBuffer<MeshData> mesh_buffer          = ResourceDescriptorHeap[buffer_indices.mesh];
    StructuredBuffer<uint> index_buffer             = ResourceDescriptorHeap[buffer_indices.index];
    StructuredBuffer<Submesh> submesh_buffer        = ResourceDescriptorHeap[buffer_indices.submesh];
    ByteAddressBuffer attribute_buffer              = ResourceDescriptorHeap[buffer_indices.attribute];
    StructuredBuffer<MaterialData> material_buffer  = ResourceDescriptorHeap[buffer_indices.material];
    StructuredBuffer<uint> material_table_buffer    = ResourceDescriptorHeap[buffer_indices.material_table];
    
    // この三角形が属するインスタンス
    uint instance_id = InstanceID();
    
    InstanceData instance = instance_buffer[instance_id];
    
    MeshData mesh = mesh_buffer[instance.mesh_index];
        
    // サブメッシュindex()
    uint submesh_index = GeometryIndex();
        
    uint material_index = material_table_buffer[mesh.material_table_index + submesh_index];
    
    MaterialData material = material_buffer[material_index];
        
    Submesh submesh = submesh_buffer[mesh.submesh_element_offset + submesh_index];
            
    uint vertex_index = mesh.index_element_offset + submesh.index_element_offset + PrimitiveIndex() * 3;

    uint index0 = index_buffer[vertex_index];
    uint index1 = index_buffer[vertex_index + 1];
    uint index2 = index_buffer[vertex_index + 2];
        
    uint normal_offset = 0;
    uint bone_offset = normal_offset + ((mesh.attribute_flags & VERTEX_ATTR_NORMALS) ? 12 : 0);
    uint uv_offset = bone_offset + ((mesh.attribute_flags & VERTEX_ATTR_BONE_WEIGHTS) ? 32 : 0);

    float3 normal = float3(0.0f, 1.0f, 0.0f);
    float2 uv = float2(0.0f, 0.0f);
        
    // 重心座標による補間
    float u = attr.barycentrics.x;
    float v = attr.barycentrics.y;
    float w = 1.0f - u - v;
    
    {
        uint addr0 = mesh.vertex_attribute_byte_offset + (index0 * mesh.vertex_stride) + normal_offset;
        uint addr1 = mesh.vertex_attribute_byte_offset + (index1 * mesh.vertex_stride) + normal_offset;
        uint addr2 = mesh.vertex_attribute_byte_offset + (index2 * mesh.vertex_stride) + normal_offset;
        normal = (asfloat(attribute_buffer.Load3(addr0)) * w) + (asfloat(attribute_buffer.Load3(addr1)) * u) + (asfloat(attribute_buffer.Load3(addr2)) * v);
    }
    
    {
        uint addr0 = mesh.vertex_attribute_byte_offset + (index0 * mesh.vertex_stride) + uv_offset;
        uint addr1 = mesh.vertex_attribute_byte_offset + (index1 * mesh.vertex_stride) + uv_offset;
        uint addr2 = mesh.vertex_attribute_byte_offset + (index2 * mesh.vertex_stride) + uv_offset;
        uv = (asfloat(attribute_buffer.Load2(addr0)) * w) + (asfloat(attribute_buffer.Load2(addr1)) * u) + (asfloat(attribute_buffer.Load2(addr2)) * v);
    }
    
    
    Texture2D base_tex = ResourceDescriptorHeap[material.base_color_tex_idx];
    Texture2D emissive_tex = ResourceDescriptorHeap[material.emissive_tex_idx];
    
    float4 base_color_sample = base_tex.SampleLevel(sam, uv, 0.0f);
    float3 emissive_sample = emissive_tex.SampleLevel(sam, uv, 0.0f).rgb;
    
    // ベースカラー = 定数基本色 * テクスチャカラー、 アルファ = 定数不透明度 * テクスチャアルファ
    float4 final_base = float4(material.props.base_color, material.props.opacity) * base_color_sample;
    
    // エミッシブ = 定数自発光色 * 定数強度 * テクスチャカラー
    float3 final_emissive = material.props.emissive_color * material.props.emissive_intensity * emissive_sample;
    
    // ベースカラーに自発光成分を加算
    float3 final_rgb = final_base.rgb + final_emissive;
        
    payload.color = final_base.xyz;
        
}