#include "Resource.hlsli"
// 最大16KBまで
struct ASPayload
{
    uint instance_id;
};

ConstantBuffer<SystemBufferIndices> buffer_indices : register(b0);

ConstantBuffer<FrameRenderConstants> frame : register(b1);

[numthreads(1, 1, 1)]
void main(uint gid : SV_GroupID)
{
    ASPayload payload;
    payload.instance_id = gid;
    
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[buffer_indices.instance];
    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[buffer_indices.mesh];

    
    InstanceData instance = instance_buffer[gid];
    MeshData mesh = mesh_buffer[instance.mesh_index];

    DispatchMesh(mesh.meshlet_count, 1, 1, payload);
}