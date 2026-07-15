struct VertexIn
{
    float3 Position;
    float4 Color;
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

struct Meshlet
{
    uint vertex_offset; //  4バイト - 頂点インデックスの開始オフセット
    uint triangle_offset; //  4バイト - 三角形インデックスの開始オフセット
    uint vertex_count; //  4バイト - このメッシュレット内の頂点数
    uint triangle_count; //  4バイト - このメッシュレット内の三角形（プリミティブ）数

    uint material_index; //  4バイト - 使用するマテリアルデータのローカルインデックス
};

struct MeshData
{
    uint vertex_position_offset; // 4バイト - 巨大バッファ内での位置（XYZ）のバイトオフセット
    uint vertex_attribute_offset; // 4バイト - 巨大バッファ内での属性（UV/Normal等）のバイトオフセット

    uint meshlet_offset;
    
    uint attribute_flags; // 4バイト - 頂点属性フラグ
    uint vertex_stride; // 4バイト - 頂点属性（1頂点あたり）のバイトサイズ（ストライド）
};

struct InstanceData
{
    float4x4 world_matrix; // 64バイト - ワールド変換行列
    uint material_table_index; //  4バイト - 使用するマテリアルテーブルのインデックス
    uint mesh_index;
};

struct HeapIndices
{
    float4x4 view_proj;
    uint position;
    uint attribute;
    uint vertex_indices;
    uint triangle_indices;
    uint meshlet;
    uint mesh;
    uint material;
    uint material_table;
    uint instance;
};
ConstantBuffer<HeapIndices> heap_indices : register(b0);

#define MAX_VERTICES 64
#define MAX_PRIMITIVES 126

// 頂点属性フラグ定義 (C++のEVertexAttributeに対応)
#define VERTEX_ATTR_NONE            (0)
#define VERTEX_ATTR_NORMALS         (1 << 0)  // 法線 (float3)
#define VERTEX_ATTR_TANGENTS        (1 << 1)  // 接線 (float3)
#define VERTEX_ATTR_COLORS          (1 << 2)  // 頂点カラー (rgba)

// スキニング（アニメーション用）
#define VERTEX_ATTR_BONE_WEIGHTS    (1 << 3)  // ボーンのブレンドウェイト (UInt4 + float4)

// モーフィング（ブレンドシェイプ／表情用）
#define VERTEX_ATTR_BASE_POSITIONS  (1 << 4)  // ベース位置
#define VERTEX_ATTR_DELTA_POSITIONS (1 << 5)  // 移動差分位置
#define VERTEX_ATTR_DELTA_NORMALS   (1 << 6)  // 移動差分法線
#define VERTEX_ATTR_DELTA_TANGENTS  (1 << 7)  // 移動差分接線

// テクスチャ座標
#define VERTEX_ATTR_TEXCOORDS0      (1 << 8)  // 通常のUV
#define VERTEX_ATTR_TEXCOORDS1      (1 << 9)  // セカンドUV（ライトマップや2枚目のマスク用）
#define VERTEX_ATTR_TEXCOORDS2      (1 << 10)
#define VERTEX_ATTR_TEXCOORDS3      (1 << 11)


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
    uint3 gid : SV_GroupID, //「自分は何番目のMeshletか」をインデックスとして使う
    out vertices VertexOut outputs[MAX_VERTICES],
    out indices uint3 triangles[MAX_PRIMITIVES]
)
{
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[heap_indices.instance];

    
    StructuredBuffer<float3> position_buffer = ResourceDescriptorHeap[heap_indices.position];
    ByteAddressBuffer attribute_buffer = ResourceDescriptorHeap[heap_indices.attribute];
    
    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[heap_indices.mesh];
    
    StructuredBuffer<Meshlet> meshlet_buffer = ResourceDescriptorHeap[heap_indices.meshlet];
    
    StructuredBuffer<uint> vertex_indices_buffer = ResourceDescriptorHeap[heap_indices.vertex_indices];
    StructuredBuffer<uint> triangle_indices_buffer = ResourceDescriptorHeap[heap_indices.triangle_indices];
   
    uint instance_id = gid.y;
    uint local_mesh_id = gid.x;
    
    InstanceData instance = instance_buffer[instance_id];
    
    MeshData mesh = mesh_buffer[instance.mesh_index];

    Meshlet meshlet = meshlet_buffer[mesh.meshlet_offset + local_mesh_id];
    
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);

    uint vertex_index_offset = meshlet.vertex_offset;
          
    float4x4 mat = mul(heap_indices.view_proj, instance.world_matrix);
    
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

    if (gtid < meshlet.triangle_count)
    {
        // パックされたUInt32データを1つ取得
        uint packedTriangle = triangle_indices_buffer[meshlet.triangle_offset + gtid];
        
        // ビットシフトとマスク演算で、1つのUInt32から3つのローカル頂点IDを切り出す
        uint idx0 = (packedTriangle >> 0) & 0xFFu;
        uint idx1 = (packedTriangle >> 8) & 0xFFu;
        uint idx2 = (packedTriangle >> 16) & 0xFFu;

        // 出力インデックス配列（uint3型）に格納
        triangles[gtid] = uint3(idx0, idx1, idx2);
    }
}
