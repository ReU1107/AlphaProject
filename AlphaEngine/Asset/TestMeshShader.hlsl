struct VertexIn
{
    float3 Position;
    float4 Color;
};

struct VertexOut
{
    float4 Position : SV_Position;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
    nointerpolation uint MaterialID : MATERIAL_ID;

};

// C++側の「meshopt_Meshlet」と完全に一致させるHLSL側の定義
struct MeshoptMeshlet
{
    uint VertexOffset;
    uint TriangleOffset;
    
    uint VertexCount;
    uint TriangleCount;
    
    uint MaterialId;
};

// 【非バインドレス】定数バッファ
struct MeshConstants
{
    float4x4 WorldViewProj;
    uint VertexHeapIndex; // 元の頂点データのバッファ
    uint VertexAttributeHeapIndex; // 元の頂点属性データのバッファ
    uint MeshletMetaHeapIndex; // global_meshlets バッファ
    uint MeshletVertHeapIndex; // global_meshlet_vertices バッファ
    uint MeshletTriHeapIndex; // global_meshlet_triangles バッファ
    uint MaterialHeapIndex; // \
    
    uint GlobalVertexPositionOffset;
    uint GlobalVertexAttributeOffset;
    
    // どこかに移動させるかも
    uint VertexStride; // 
    uint AttributeFlags; // 
    
};
ConstantBuffer<MeshConstants> cb : register(b0);

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
    StructuredBuffer<float3> vertexBuffer = ResourceDescriptorHeap[cb.VertexHeapIndex];
    StructuredBuffer<MeshoptMeshlet> meshletMetaBuffer = ResourceDescriptorHeap[cb.MeshletMetaHeapIndex];
    StructuredBuffer<uint> meshletVertBuffer = ResourceDescriptorHeap[cb.MeshletVertHeapIndex];
    StructuredBuffer<uint> meshletTriBuffer= ResourceDescriptorHeap[cb.MeshletTriHeapIndex];

    MeshoptMeshlet m;
    m.VertexOffset = meshletMetaBuffer[gid].VertexOffset;
    m.TriangleOffset = meshletMetaBuffer[gid].TriangleOffset;
    m.VertexCount = meshletMetaBuffer[gid].VertexCount;
    m.TriangleCount = meshletMetaBuffer[gid].TriangleCount;
    m.MaterialId = meshletMetaBuffer[gid].MaterialId;

    // このMeshletの実際の頂点数・三角形数を確定させる
    SetMeshOutputCounts(m.VertexCount, m.TriangleCount);

    uint vertexIndexOffset = m.VertexOffset;
    
    uint vertexCount = m.VertexCount;
    
    if (gtid < vertexCount)
    {
        // まず、この頂点が「元々のモデルで何番目の頂点だったか」をインデックスバッファから引く
        uint vertexIndex = meshletVertBuffer[vertexIndexOffset + gtid];
        
        // 位置は元々の頂点番号で引く
        float3 rawPos = vertexBuffer[cb.GlobalVertexPositionOffset + vertexIndex];

        // 頂点属性（ByteAddressBuffer）の正しいロード処理
        ByteAddressBuffer vertexAttributeBuffer = ResourceDescriptorHeap[cb.VertexAttributeHeapIndex];

        // 頂点属性が入っているであろう先頭アドレスからこの頂点index分オフセット
        uint baseAddress = cb.GlobalVertexAttributeOffset + (vertexIndex * cb.VertexStride);

        // （※以下、マクロやオフセット計算は先ほどのままで完全に動作します）
#define VERTEX_ATTR_NORMALS      1u
#define VERTEX_ATTR_BONE_WEIGHTS 8u
#define VERTEX_ATTR_TEXCOORDS0   256u

        uint normalOffset = 0;
        uint boneOffset = normalOffset + ((cb.AttributeFlags & VERTEX_ATTR_NORMALS) ? 12 : 0);
        uint uvOffset = boneOffset + ((cb.AttributeFlags & VERTEX_ATTR_BONE_WEIGHTS) ? 32 : 0);

        float3 normal = float3(0.0f, 1.0f, 0.0f);
        float2 uv = float2(0.0f, 0.0f);

        // 法線のロード（個別asfloat）
        if (cb.AttributeFlags & VERTEX_ATTR_NORMALS)
        {
            uint addr = baseAddress + normalOffset;
            normal = asfloat(vertexAttributeBuffer.Load3(addr));

        }

        // UVのロード（個別asfloat）
        if (cb.AttributeFlags & VERTEX_ATTR_TEXCOORDS0)
        {
            uint addr = baseAddress + uvOffset;
            uv = asfloat(vertexAttributeBuffer.Load2(addr));

        }

        // 座標変換して出力
        balls[gtid].Position = mul(cb.WorldViewProj, float4(rawPos, 1.0f));
        
        // ★ 今度こそ、全身の法線が1ピクセルの狂いもなく滑らかに繋がります！
        //balls[gtid].Color = float4(normal * 0.5f + 0.5f, 1.0f);
        balls[gtid].Color = float4(uv, 0.0f, 1.0f);
        balls[gtid].TexCoord = uv;
        balls[gtid].MaterialID = m.MaterialId;
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
