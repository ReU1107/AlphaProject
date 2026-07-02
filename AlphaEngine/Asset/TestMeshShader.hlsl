struct VertexIn
{
    float3 Position;
    float4 Color;
};

struct VertexOut
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
};

// C++側の「meshopt_Meshlet」と完全に一致させるHLSL側の定義
struct MeshoptMeshlet
{
    uint VertexOffset;
    uint TriangleOffset;
    uint VertexCount;
    uint TriangleCount;
    
    uint material_id;
    uint instance_id;
};

// 【非バインドレス】定数バッファ
struct MeshConstants
{
    float4x4 WorldViewProj;
    uint VertexHeapIndex; // 元の頂点データのバッファ
    uint MeshletMetaHeapIndex; // global_meshlets バッファ
    uint MeshletVertHeapIndex; // global_meshlet_vertices バッファ
    uint MeshletTriHeapIndex; // global_meshlet_triangles バッファ
};
ConstantBuffer<MeshConstants> cb : register(b0);

#define MAX_VERTICES 64
#define MAX_PRIMITIVES 126

float3 GetRandomColor(uint id)
{
    uint h = id * 2747636419u;
    h = (h ^ (h >> 16)) * 2747636419u;
    return float3((h & 0xFFu), ((h >> 8u) & 0xFFu), ((h >> 16u) & 0xFFu)) / 255.0f;
}

// ==========================================
// メッシュシェーダー (MS)
// ==========================================
[NumThreads(128, 1, 1)]
[OutputTopology("triangle")]
void main(
    uint gtid : SV_GroupThreadID,
    uint gid : SV_GroupID, // ★「自分は何番目のMeshletか」をインデックスとして使う
    out vertices VertexOut balls[MAX_VERTICES],
    out indices uint3 triangles[MAX_PRIMITIVES]
)
{
    // 1. 各バッファを Bindless Heap から ByteAddressBuffer としてフェッチ
    ByteAddressBuffer vertexBuffer = ResourceDescriptorHeap[cb.VertexHeapIndex];
    StructuredBuffer<MeshoptMeshlet> meshletMetaBuffer = ResourceDescriptorHeap[cb.MeshletMetaHeapIndex];
    StructuredBuffer<uint> meshletVertBuffer = ResourceDescriptorHeap[cb.MeshletVertHeapIndex];
    StructuredBuffer<uint> meshletTriBuffer= ResourceDescriptorHeap[cb.MeshletTriHeapIndex];

    MeshoptMeshlet m;
    m.VertexOffset = meshletMetaBuffer[gid].VertexOffset;
    m.TriangleOffset = meshletMetaBuffer[gid].TriangleOffset;
    m.VertexCount = meshletMetaBuffer[gid].VertexCount;
    m.TriangleCount = meshletMetaBuffer[gid].TriangleCount;

    // このMeshletの実際の頂点数・三角形数を確定させる
    SetMeshOutputCounts(m.VertexCount, m.TriangleCount);

    if (gtid < m.VertexCount)
    {
        uint vertexIndex = meshletVertBuffer[m.VertexOffset + gtid];

        // 元の頂点バッファから座標を引く (Vector3=12バイト
        uint vertexAddress = vertexIndex * 12;
        float3 rawPos = asfloat(vertexBuffer.Load3(vertexAddress));

        // 座標変換して出力
        balls[gtid].Position = mul(cb.WorldViewProj, float4(rawPos, 1.0f));
        balls[gtid].Color = float4(GetRandomColor(gid), 1.0f);

    }

    // meshoptimizer の triangles は1頂点1バイト（unsigned char）のフラット配列です
    if (gtid < m.TriangleCount)
    {
        // パックされたUInt32データを1つ取得
        uint packedTriangle = meshletTriBuffer[m.TriangleOffset + gtid];
        
        // ビットシフトとマスク演算で、1つのUInt32から3つのローカル頂点IDを切り出す
        uint idx0 = (packedTriangle >> 0) & 0xFFu;
        uint idx1 = (packedTriangle >> 8) & 0xFFu;
        uint idx2 = (packedTriangle >> 16) & 0xFFu;

        // 出力インデックス配列（uint3型）に格納
        triangles[gtid] = uint3(idx0, idx1, idx2);
    }
}
