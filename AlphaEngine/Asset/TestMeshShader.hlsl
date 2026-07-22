struct ASPayload
{
    uint instance_id;
};

struct VertexOut
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    nointerpolation uint MaterialTableID : MATERIAL_TABLE_ID;
    nointerpolation uint MaterialID : MATERIAL_ID;

};

#include "Resource.hlsli"

ConstantBuffer<SystemBufferIndices> buffer_indices : register(b0);
ConstantBuffer<FrameRenderContext> frame : register(b1);


float3 GetRandomColor(uint id)
{
    uint h = id * 2747636419u;
    h = (h ^ (h >> 16)) * 2747636419u;
    return float3((h & 0xFFu), ((h >> 8u) & 0xFFu), ((h >> 16u) & 0xFFu)) / 255.0f;
}

[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void main(
    uint gtid : SV_GroupThreadID,
    uint gid : SV_GroupID,
    in payload ASPayload payload,
    out vertices VertexOut outputs[MAX_VERTICES],
    out indices uint3 triangles[MAX_PRIMITIVES]
)
{
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[buffer_indices.instance];

    StructuredBuffer<float3> position_buffer = ResourceDescriptorHeap[buffer_indices.position];
    ByteAddressBuffer attribute_buffer = ResourceDescriptorHeap[buffer_indices.attribute];
    
    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[buffer_indices.mesh];
    
    StructuredBuffer<Meshlet> meshlet_buffer = ResourceDescriptorHeap[buffer_indices.meshlet];
    
    StructuredBuffer<uint> vertex_indices_buffer = ResourceDescriptorHeap[buffer_indices.vertex_indices];
    StructuredBuffer<uint> triangle_indices_buffer = ResourceDescriptorHeap[buffer_indices.triangle_indices];
   
    uint instance_id = payload.instance_id;
    uint local_mesh_id = gid;
    
    InstanceData instance = instance_buffer[instance_id];
    
    MeshData mesh = mesh_buffer[instance.mesh_index];

    Meshlet meshlet = meshlet_buffer[mesh.meshlet_offset + local_mesh_id];
    
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);

    uint vertex_index_offset = mesh.vertex_offset + meshlet.vertex_offset;
    
    
    
    float4x4 mat = mul(frame.view_proj, instance.world_matrix);
    
    if (gtid < meshlet.vertex_count)
    {
        // まず、この頂点が「元々のモデルで何番目の頂点だったか」をインデックスバッファから引く
        uint vertex_index = vertex_indices_buffer[vertex_index_offset + gtid];
                
        // 位置は元々の頂点番号で引く
        float3 position = position_buffer[mesh.vertex_position_offset + vertex_index];

        // 頂点属性が入っているであろう先頭アドレスからこの頂点index分オフセット
        uint base_offset = mesh.vertex_attribute_offset + (vertex_index * mesh.vertex_stride);

        uint normal_offset = 0;
        uint bone_offset = normal_offset + ((mesh.attribute_flags & VERTEX_ATTR_NORMALS) ? 12 : 0);
        uint uv_offset = bone_offset + ((mesh.attribute_flags & VERTEX_ATTR_BONE_WEIGHTS) ? 32 : 0);

        float3 normal = float3(0.0f, 1.0f, 0.0f);
        float2 uv = float2(0.0f, 0.0f);

        if (mesh.attribute_flags & VERTEX_ATTR_NORMALS)
        {
            uint addr = base_offset + normal_offset;
            normal = asfloat(attribute_buffer.Load3(addr));

        }
        if (mesh.attribute_flags & VERTEX_ATTR_TEXCOORDS0)
        {
            uint addr = base_offset + uv_offset;
            uv = asfloat(attribute_buffer.Load2(addr));

        }
        
        // 座標変換して出力
        outputs[gtid].Position = mul(mat, float4(position, 1.0f));
        
        outputs[gtid].Color = float4(uv, 0.0f, 1.0f);
        outputs[gtid].TexCoord = uv;
        outputs[gtid].Normal = normal;
        outputs[gtid].MaterialTableID = instance.material_table_index;
        outputs[gtid].MaterialID = meshlet.material_index;
    }

    uint triangle_index_offset = mesh.triangle_offset + meshlet.triangle_offset;
    
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
    }
}
