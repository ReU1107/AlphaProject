
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
    uint position;
    uint attribute;
    uint vertex_indices;
    uint triangle_indices;
    uint meshlet;
    uint mesh;
    uint material;
    uint material_table;
};

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
