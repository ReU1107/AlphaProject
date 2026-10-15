#include "Resource.hlsli"


// それぞれの頂点が自分の頂点を処理するようにしてみる

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
 
    uint vertex_index = id.x;

    
    StructuredBuffer<uint> vertex_to_instance_buffer  = ResourceDescriptorHeap[render_buffers.vertex_to_instance];
    StructuredBuffer<InstanceData> instance_buffer    = ResourceDescriptorHeap[render_buffers.instance];
    StructuredBuffer<float3> position_buffer          = ResourceDescriptorHeap[render_buffers.position];
    ByteAddressBuffer attribute_buffer                = ResourceDescriptorHeap[render_buffers.attribute];
    RWStructuredBuffer<float3> output_position_buffer = ResourceDescriptorHeap[render_buffers.current_position_uav];
    StructuredBuffer<MeshData> mesh_buffer            = ResourceDescriptorHeap[render_buffers.mesh];
    StructuredBuffer<float4x4> matrix_palette_buffer  = ResourceDescriptorHeap[render_buffers.matrix_palette];
    
    uint instance_index = vertex_to_instance_buffer[vertex_index];
    InstanceData instance = instance_buffer[instance_index];
    
    MeshData mesh = mesh_buffer[instance.mesh_index];
    
    uint local_vertex_index = vertex_index - instance.vertex_element_offset;
    
    float3 base_position = position_buffer[mesh.vertex_position_element_offset + local_vertex_index];

    if (instance.matrix_palette_offset != INVALID_INDEX)
    {
        // 頂点属性が入っているであろう先頭アドレスからこの頂点index分オフセット
        uint base_offset = mesh.vertex_attribute_byte_offset + (local_vertex_index * mesh.vertex_stride);

        uint normal_offset = 0;
        uint bone_offset = normal_offset + ((mesh.attribute_flags & VERTEX_ATTR_NORMALS) ? 12 : 0);
        uint uv_offset = bone_offset + ((mesh.attribute_flags & VERTEX_ATTR_BONE_WEIGHTS) ? 32 : 0);

        float3 normal = float3(0.0f, 1.0f, 0.0f);
        uint4 bone_index = uint4(0, 0, 0, 0);
        float4 bone_weight = float4(0.0f, 0.0f, 0.0f, 0.0f);

        if (mesh.attribute_flags & VERTEX_ATTR_NORMALS)
        {
            uint addr = base_offset + normal_offset;
            normal = asfloat(attribute_buffer.Load3(addr));

        }
        if (mesh.attribute_flags & VERTEX_ATTR_BONE_WEIGHTS)
        {
            uint addr = base_offset + bone_offset;
            bone_index = asuint(attribute_buffer.Load4(addr));
            bone_weight = asfloat(attribute_buffer.Load4(addr + 16));
        }
        
        float4x4 skinned_mat = 0;
        skinned_mat += matrix_palette_buffer[instance.matrix_palette_offset + bone_index.x] * bone_weight.x;
        skinned_mat += matrix_palette_buffer[instance.matrix_palette_offset + bone_index.y] * bone_weight.y;
        skinned_mat += matrix_palette_buffer[instance.matrix_palette_offset + bone_index.z] * bone_weight.z;
        skinned_mat += matrix_palette_buffer[instance.matrix_palette_offset + bone_index.w] * bone_weight.w;
    
        
        base_position = mul(skinned_mat, float4(base_position, 1.0f)).xyz;

    }
    
    output_position_buffer[vertex_index] = base_position;
}