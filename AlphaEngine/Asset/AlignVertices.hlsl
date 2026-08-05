#include "Resource.hlsli"


ConstantBuffer<SystemBufferIndices> buffer_indices : register(b0);

// それぞれの頂点が自分の頂点を処理するようにしてみる

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
 
    uint vertex_index = id.x;

    StructuredBuffer<uint> vertex_to_instance_buffer  = ResourceDescriptorHeap[buffer_indices.vertex_to_instance];
    StructuredBuffer<InstanceData> instance_buffer    = ResourceDescriptorHeap[buffer_indices.instance];
    StructuredBuffer<float3> position_buffer          = ResourceDescriptorHeap[buffer_indices.position];
    ByteAddressBuffer attribute_buffer                = ResourceDescriptorHeap[buffer_indices.attribute];
    RWStructuredBuffer<float3> output_position_buffer = ResourceDescriptorHeap[buffer_indices.current_position_uav];
    StructuredBuffer<MeshData> mesh_buffer            = ResourceDescriptorHeap[buffer_indices.mesh];
    
    uint instance_index = vertex_to_instance_buffer[vertex_index];
    InstanceData instance = instance_buffer[instance_index];
    
    MeshData mesh = mesh_buffer[instance.mesh_index];
    
    uint local_vertex_index = vertex_index - instance.vertex_element_offset;
    
    
    float3 base_position = position_buffer[mesh.vertex_position_element_offset + local_vertex_index];
    
    /*
    ここでスキニングとかを行うとりあえずそのまま流す
    */
   
    output_position_buffer[vertex_index] = base_position;
}