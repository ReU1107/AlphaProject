#include "Resource.hlsli"

struct TaskData
{
    uint instance_index;
};


ConstantBuffer<HeapIndices> heap_indices : register(b0);

// 頂点シェーダーからピクセルシェーダーへ渡すデータ
struct VertexOutput
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    nointerpolation uint material_table_id : MATERIAL_TABLE_ID;
    nointerpolation uint material_id : MATERIAL_ID;
    
};


//[Shader("node")]
//[NodeLaunch("coalescing")]
[Shader("node")] 
[NodeLaunch("mesh")]
[NodeDispatchGrid(1, 1, 1)]
[NumThreads(MAX_VERTICES, 1, 1)]
[OutputTopology("triangle")]
void MeshNode(
    uint thread_index : SV_GroupThreadID,
    GroupNodeInputRecords< TaskData> input,
    // メッシュシェーダー特有の出力バッファ（頂点配列とインデックス配列）
    out vertices VertexOutput outputs[MAX_VERTICES],
    out indices uint3 triangles[MAX_PRIMITIVES])
{
    // 前段の EntryNode から流れてきたデータを取り出す
    TaskData data = input.Get();
    
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[heap_indices.instance];

    StructuredBuffer<float3> position_buffer = ResourceDescriptorHeap[heap_indices.position];
    ByteAddressBuffer attribute_buffer = ResourceDescriptorHeap[heap_indices.attribute];
    
    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[heap_indices.mesh];
    
    StructuredBuffer<Meshlet> meshlet_buffer = ResourceDescriptorHeap[heap_indices.meshlet];
    
    StructuredBuffer<uint> vertex_indices_buffer = ResourceDescriptorHeap[heap_indices.vertex_indices];
    StructuredBuffer<uint> triangle_indices_buffer = ResourceDescriptorHeap[heap_indices.triangle_indices];
   
    uint instance_id = data.instance_index;
    uint mesh_let_index = thread_index;
    
    InstanceData instance = instance_buffer[instance_id];
    
    MeshData mesh = mesh_buffer[instance.mesh_index];

    Meshlet meshlet = meshlet_buffer[mesh.meshlet_offset + mesh_let_index];
    
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);

    uint vertex_index_offset = meshlet.vertex_offset;
          
    float4x4 mat = mul(heap_indices.view_proj, instance.world_matrix);
    
    if (mesh_let_index < meshlet.vertex_count)
    {
        // まず、この頂点が「元々のモデルで何番目の頂点だったか」をインデックスバッファから引く
        uint vertex_index = vertex_indices_buffer[vertex_index_offset + mesh_let_index];
                
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
        outputs[mesh_let_index].position = mul(mat, float4(position, 1.0f));
        
        outputs[mesh_let_index].color = float4(uv, 0.0f, 1.0f);
        outputs[mesh_let_index].texcoord = uv;
        outputs[mesh_let_index].normal = normal;
        outputs[mesh_let_index].material_table_id = instance.material_table_index;
        outputs[mesh_let_index].material_id = meshlet.material_index;
    }

    if (mesh_let_index < meshlet.triangle_count)
    {
        // パックされたUInt32データを1つ取得
        uint packedTriangle = triangle_indices_buffer[meshlet.triangle_offset + mesh_let_index];
        
        // ビットシフトとマスク演算で、1つのUInt32から3つのローカル頂点IDを切り出す
        uint idx0 = (packedTriangle >> 0) & 0xFFu;
        uint idx1 = (packedTriangle >> 8) & 0xFFu;
        uint idx2 = (packedTriangle >> 16) & 0xFFu;

        // 出力インデックス配列（uint3型）に格納
        triangles[mesh_let_index] = uint3(idx0, idx1, idx2);
    }
    
}