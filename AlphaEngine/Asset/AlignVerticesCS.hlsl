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