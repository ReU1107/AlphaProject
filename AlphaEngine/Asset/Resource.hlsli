#ifndef ALPHA_RESOURCE_HLSLI
#define ALPHA_RESOURCE_HLSLI


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

struct TextureBindingData
{
    uint texture_index;
    uint sampler_index;
};

struct MaterialData
{
    MaterialProperties props;

    TextureBindingData base_color;
    TextureBindingData normal;
    TextureBindingData metallic_roughness;
    TextureBindingData emissive;
    TextureBindingData occlusion;
};

struct Meshlet
{
    uint vertex_element_offset;     // 頂点インデックスの開始オフセット
    uint triangle_element_offset;   // 三角形インデックスの開始オフセット
    uint vertex_count;              // このメッシュレット内の頂点数
    uint triangle_count;            // このメッシュレット内の三角形（プリミティブ）数

    uint material_index;            // 使用するマテリアルデータのローカルインデックス
};

struct Submesh
{
    uint index_element_offset;      // 
    uint index_count;               // 使わんけどcppで使うから合わせるため
};

struct MeshData
{
    uint vertex_position_element_offset;   // 位置（XYZ）のオフセット
    uint vertex_attribute_byte_offset;     // 属性（UV/Normal等）オフセット

    uint index_element_offset;      // index
    uint submesh_element_offset;    // サブメッシュ
    
    uint meshlet_element_offset;    // メッシュレット
    uint meshlet_count;             // メッシュレット数
    
    uint vertex_element_offset;     // メッシュレット頂点インデックスのオフセット
    uint triangle_element_offset;   // メッシュレット三角形インデックスのオフセット
    
    uint material_table_index;      // 使用するマテリアルテーブルのインデックス
    
    uint attribute_flags;           // 頂点属性フラグ
    uint vertex_stride;             // 頂点属性ストライド
    uint index_count;               // 頂点
};

struct InstanceData
{
    float4x4 world_matrix;      // ワールド変換行列
    
    uint mesh_index;            //
    uint vertex_element_offset; // 実際に描画に使用される頂点バッファオフセット
    uint override_material_table_offset; // overrideのマテリアルテーブルのインデックス
    
    uint matrix_palette_offset;
    
    uint visible;
};

struct LightData
{
    float3 position;
    float radius;
    float3 color;
    float intensity;

    float3 direction;
    uint type;

    float inner_cone;
    float outer_cone;

};

struct VolumeData
{
    float3 min;
    float3 max;
    
    float density;
    float anisotropy;
    float absorption;
    float scattering;
};

struct RenderBufferIndices
{
    // Static
    uint position;              // このpositionはあくまでも読み込み時の状態の
    uint attribute;             // 頂点属性
    uint index;
    uint vertex_indices;
    uint triangle_indices;
    uint meshlet;
    uint mesh;
    uint submesh;
    uint material;
    uint material_table;
    
    
    // Dynamic
    uint instance;
    uint vertex_to_instance;
    uint current_position_srv;  // 描画時に使用するのはこっち
    uint current_position_uav;  // csから書き込む時用
    
    uint matrix_palette;
    
    uint override_material;
    
    uint light;
    uint volume;
    
    uint rt_structure;
};

struct FrameRenderConstants
{
    float4x4 view;
    float4x4 view_proj;
    float4x4 inv_view_proj; // inv_view_proj
    float4 camera_pos;
    float4 frustum_planes;
    
    float3 camera_up;
    uint instance_count;
    
    float3 camera_right;
    uint light_count;
    uint main_light;
    uint debug_value;
};


#define MAX_VERTICES 64
#define MAX_PRIMITIVES 126

#define INVALID_INDEX 0xFFFFFFFF

// 頂点属性フラグ定義 (C++のEVertexAttributeに対応)
#define VERTEX_ATTR_NONE            (0)
#define VERTEX_ATTR_NORMALS         (1 << 0)  // 法線 (float3)
#define VERTEX_ATTR_TANGENTS        (1 << 1)  // 接線 (float3)
#define VERTEX_ATTR_COLORS          (1 << 2)  // 頂点カラー (rgba)

// スキニング (アニメーション用)
#define VERTEX_ATTR_BONE_WEIGHTS    (1 << 3)  // ボーンのブレンドウェイト (UInt4 + float4)

// モーフィング (ブレンドシェイプ／表情用)
#define VERTEX_ATTR_BASE_POSITIONS  (1 << 4)  // ベース位置
#define VERTEX_ATTR_DELTA_POSITIONS (1 << 5)  // 移動差分位置
#define VERTEX_ATTR_DELTA_NORMALS   (1 << 6)  // 移動差分法線
#define VERTEX_ATTR_DELTA_TANGENTS  (1 << 7)  // 移動差分接線

// テクスチャ座標
#define VERTEX_ATTR_TEXCOORDS0      (1 << 8)  // 通常のUV
#define VERTEX_ATTR_TEXCOORDS1      (1 << 9)  // セカンドUV（ライトマップや2枚目のマスク用）
#define VERTEX_ATTR_TEXCOORDS2      (1 << 10)
#define VERTEX_ATTR_TEXCOORDS3      (1 << 11)

// サンプラー (ESamplerPresetTypeと同じ順序)
static const uint SAMPLER_PRESET_POINT_CLAMP = 0;
static const uint SAMPLER_PRESET_POINT_CLAMP_CLAMP_WRAP = 1;
static const uint SAMPLER_PRESET_POINT_CLAMP_WRAP_CLAMP = 2;
static const uint SAMPLER_PRESET_POINT_CLAMP_WRAP_WRAP = 3;
static const uint SAMPLER_PRESET_POINT_WRAP_CLAMP_CLAMP = 4;
static const uint SAMPLER_PRESET_POINT_WRAP_CLAMP_WRAP = 5;
static const uint SAMPLER_PRESET_POINT_WRAP_WRAP_CLAMP = 6;
static const uint SAMPLER_PRESET_POINT_WRAP = 7;

static const uint SAMPLER_PRESET_LINEAR_CLAMP = 8;
static const uint SAMPLER_PRESET_LINEAR_CLAMP_CLAMP_WRAP = 9;
static const uint SAMPLER_PRESET_LINEAR_CLAMP_WRAP_CLAMP = 10;
static const uint SAMPLER_PRESET_LINEAR_CLAMP_WRAP_WRAP = 11;
static const uint SAMPLER_PRESET_LINEAR_WRAP_CLAMP_CLAMP = 12;
static const uint SAMPLER_PRESET_LINEAR_WRAP_CLAMP_WRAP = 13;
static const uint SAMPLER_PRESET_LINEAR_WRAP_WRAP_CLAMP = 14;
static const uint SAMPLER_PRESET_LINEAR_WRAP = 15;

static const uint SAMPLER_PRESET_ANISO_CLAMP = 16;
static const uint SAMPLER_PRESET_ANISO_CLAMP_CLAMP_WRAP = 17;
static const uint SAMPLER_PRESET_ANISO_CLAMP_WRAP_CLAMP = 18;
static const uint SAMPLER_PRESET_ANISO_CLAMP_WRAP_WRAP = 19;
static const uint SAMPLER_PRESET_ANISO_WRAP_CLAMP_CLAMP = 20;
static const uint SAMPLER_PRESET_ANISO_WRAP_CLAMP_WRAP = 21;
static const uint SAMPLER_PRESET_ANISO_WRAP_WRAP_CLAMP = 22;
static const uint SAMPLER_PRESET_ANISO_WRAP = 23;

static const uint SAMPLER_PRESET_SHADOW = 24;


ConstantBuffer<RenderBufferIndices> render_buffers : register(b0);
ConstantBuffer<FrameRenderConstants> render_frame : register(b1);


#endif