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

    uint instance_id : CV_Instance_ID;
    uint meshlet_id : CV_Meshlet_ID;        // グローバルメッシュレットにする
};

#include "Resource.hlsli"

ConstantBuffer<SystemBufferIndices> buffer_indices : register(b0);
ConstantBuffer<FrameRenderContext> frame : register(b1);

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
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[buffer_indices.instance];

    StructuredBuffer<float3> position_buffer = ResourceDescriptorHeap[buffer_indices.current_position_srv];
    
    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[buffer_indices.mesh];
    
    StructuredBuffer<Meshlet> meshlet_buffer = ResourceDescriptorHeap[buffer_indices.meshlet];
    
    StructuredBuffer<uint> vertex_indices_buffer = ResourceDescriptorHeap[buffer_indices.vertex_indices];
    StructuredBuffer<uint> triangle_indices_buffer = ResourceDescriptorHeap[buffer_indices.triangle_indices];
   
    uint instance_id = payload.instance_id;
    uint local_mesh_id = gid;
    
    InstanceData instance = instance_buffer[instance_id];
    
    MeshData mesh = mesh_buffer[instance.mesh_index];

    uint meshlet_id = mesh.meshlet_element_offset + local_mesh_id;
    
    Meshlet meshlet = meshlet_buffer[meshlet_id];
    
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);

    uint vertex_index_offset = mesh.vertex_element_offset + meshlet.vertex_element_offset;
        
    float4x4 mat = mul(frame.view_proj, instance.world_matrix);
    
    if (gtid < meshlet.vertex_count)
    {
        // まず、この頂点が「元々のモデルで何番目の頂点だったか」をインデックスバッファから引く
        uint vertex_index = vertex_indices_buffer[vertex_index_offset + gtid];
                
        float3 position = position_buffer[instance.vertex_element_offset + vertex_index];
        
        // 座標変換して出力
        outputs[gtid].position = mul(mat, float4(position, 1.0f));
    }

    uint triangle_index_offset = mesh.triangle_element_offset + meshlet.triangle_element_offset;
    
    if (gtid < meshlet.triangle_count)
    {
        // パックされたUInt32データを1つ取得
        uint packedTriangle = triangle_indices_buffer[triangle_index_offset + gtid];
        
        // ビットシフトとマスク演算で、1つのUInt32から3つのローカル頂点IDを切り出す
        uint idx0 = (packedTriangle >> 0) & 0xFFu;
        uint idx1 = (packedTriangle >> 8) & 0xFFu;
        uint idx2 = (packedTriangle >> 16) & 0xFFu;

        // 出力インデックス配列（uint3型）に格納
        triangles[gtid] = uint3(idx0, idx1, idx2);
        prims[gtid].primitive_id = gtid;
        prims[gtid].instance_id = instance_id;
        prims[gtid].meshlet_id = local_mesh_id;

    }
}