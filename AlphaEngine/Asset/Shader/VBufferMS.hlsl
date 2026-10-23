struct ASPayload
{
    uint instance_id;
};

struct MeshOut
{
    float4 position : SV_Position;

};

// visibilitybufferを64bitでやるから

struct PrimitiveAttribute
{
    uint primitive_id : SV_PrimitiveID;

    uint instance_id : CV_InstanceID;
    uint meshlet_id : CV_MeshletID;        // グローバルメッシュレットにする
};

#include "Resource.hlsli"



[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void main(
    uint gtid : SV_GroupThreadID,
    uint gid : SV_GroupID,
    in payload ASPayload payload,
    out vertices MeshOut outputs[MAX_VERTICES],
    out indices uint3 triangles[MAX_PRIMITIVES],
    out primitives PrimitiveAttribute prims[MAX_PRIMITIVES]
)
{
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[render_buffers.instance];
    
    uint instance_id = payload.instance_id;

        
    InstanceData instance = instance_buffer[instance_id];
    
    if (instance.visible == 0)
        return;
    
    
    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[render_buffers.mesh];
    
    StructuredBuffer<Meshlet> meshlet_buffer = ResourceDescriptorHeap[render_buffers.meshlet];
    
    StructuredBuffer<uint> vertex_indices_buffer = ResourceDescriptorHeap[render_buffers.vertex_indices];
    StructuredBuffer<uint> triangle_indices_buffer = ResourceDescriptorHeap[render_buffers.triangle_indices];
   
    uint local_meshlet_id = gid;

    
    StructuredBuffer<float3> position_buffer = ResourceDescriptorHeap[instance.vertex_position_index];
    
    MeshData mesh = mesh_buffer[instance.mesh_index];

    uint meshlet_id = mesh.meshlet_element_offset + local_meshlet_id;
    
    Meshlet meshlet = meshlet_buffer[meshlet_id];
    
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);

    uint vertex_index_offset = mesh.vertex_element_offset + meshlet.vertex_element_offset;
        
    float4x4 mat = mul(render_frame.view_proj, instance.world_matrix);
    
    if (gtid < meshlet.vertex_count)
    {
        // まず、この頂点が「元々のモデルで何番目の頂点だったか」をインデックスバッファから引く
        uint vertex_index = vertex_indices_buffer[vertex_index_offset + gtid];
                
        float3 position = position_buffer[instance.vertex_position_element_offset + vertex_index];
        
        outputs[gtid].position = mul(mat, float4(position, 1.0f));
    }

    uint triangle_index_offset = mesh.triangle_element_offset + meshlet.triangle_element_offset;
    
    if (gtid < meshlet.triangle_count)
    {
        // パックされたUInt32データを1つ取得
        uint triangle_index = triangle_indices_buffer[triangle_index_offset + gtid];
        
        // ビットシフトとマスク演算で、1つのUInt32から3つのローカル頂点IDを切り出す
        uint idx0 = (triangle_index >> 0) & 0xFFu;
        uint idx1 = (triangle_index >> 8) & 0xFFu;
        uint idx2 = (triangle_index >> 16) & 0xFFu;
        
        // 出力インデックス配列（uint3型）に格納
        triangles[gtid] = uint3(idx0, idx1, idx2);
        prims[gtid].primitive_id = gtid;
        prims[gtid].instance_id = instance_id;
        prims[gtid].meshlet_id = local_meshlet_id;

    }
}