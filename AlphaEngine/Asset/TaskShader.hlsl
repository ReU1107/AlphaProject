#include "Resource.hlsli"
// 最大16KBまで
struct ASPayload
{
    uint instance_id;
};

[numthreads(1, 1, 1)]
void main(uint gid : SV_GroupID)
{
    ASPayload payload;
    payload.instance_id = gid;
    
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[render_buffers.instance];
    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[render_buffers.mesh];

    
    InstanceData instance = instance_buffer[gid];
    MeshData mesh = mesh_buffer[instance.mesh_index];

    DispatchMesh(mesh.meshlet_count, 1, 1, payload);
}