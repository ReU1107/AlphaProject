
struct MaterialProperties
{
    float3 base_color;          // 基本色
    float opacity;              // 不透明度

    float metallic;             // 金属感
    float roughness;            // 粗さ
    float emissive_intensity;   // 自発光の強さ
    float occlusion;            // アンビエントオクルージョン

    float3 emissive_color;      // 自発光の色
    float refraction_index;     // 屈折率

    float3 specular_tint;       // 鏡面反射
};

struct MaterialData
{
    MaterialProperties props; // 

    uint base_color_tex_idx;            // テクスチャバインドレスインデックス (0xFFFFFFFF = 無効)
    uint normal_tex_idx;                // 法線マップインデックス
    uint metallic_roughness_tex_idx;    // 
    uint emissive_tex_idx;              //  

    uint occlusion_tex_idx;             // 
    uint toon_tex_idx;                  // MMD等のトゥーンテクスチャ用
    uint sphere_tex_idx;                // MMD等のスフィアマップ用
    uint flags;                         // マテリアル判別フラグ
};

struct Meshlet
{
    uint vertex_element_offset;     // 頂点インデックスの開始オフセット
    uint triangle_element_offset;   // 三角形インデックスの開始オフセット
    uint vertex_count;              // このメッシュレット内の頂点数
    uint triangle_count;            // このメッシュレット内の三角形（プリミティブ）数

    uint material_index;            // 使用するマテリアルデータのローカルインデックス
};

struct MeshData
{
    uint vertex_position_element_offset;   // 位置（XYZ）のオフセット
    uint vertex_attribute_byte_offset;     // 属性（UV/Normal等）オフセット

    uint meshlet_element_offset;    // メッシュレット
    uint meshlet_count;             // メッシュレット数
    
    uint vertex_element_offset;     // メッシュレット頂点インデックスのオフセット
    uint triangle_element_offset;   // メッシュレット三角形インデックスのオフセット
    
    uint attribute_flags;           // 頂点属性フラグ
    uint vertex_stride;             // 頂点属性ストライド
};

struct InstanceData
{
    float4x4 world_matrix;      // ワールド変換行列
    
    uint material_table_index;  // 使用するマテリアルテーブルのインデックス
    uint mesh_index;
    
    uint vertex_element_offset;         // 実際に描画に使用される頂点バッファオフセット
};

struct SystemBufferIndices
{
    uint position;              // このpositionはあくまでも読み込み時の状態の
    uint attribute;             // 頂点属性
    uint vertex_indices;
    uint triangle_indices;
    uint meshlet;
    uint mesh;
    uint material;
    uint material_table;
    uint instance;
    uint vertex_to_instance;
    uint dyanmic_position_srv;      // csから書き込む時用
    uint dyanmic_position_uav;      // 描画時に使用するのはこっち
};

struct FrameRenderContext
{
    float4x4 view_proj;
    float4 camera_pos;
    float4 frustum_planes;

    uint instance_num;
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
