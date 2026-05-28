

struct CameraData
{
    float4x4 viewProjectionMatrix; // ビュープロジェクション変換行列
};

ConstantBuffer<CameraData> Camera : register(b0);

struct SkeletalVertex
{
    float3 position     : POSITION;
    float3 normal       : NORMAL;
    float2 texcoord     : TEXCOORD;
    uint4 bone_index    : BONEINDEX;
    float4 bone_weight  : BONEWEIGHT;
};

StructuredBuffer<float4x4> BonePalette : register(t0);


struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal   : NORMAl;
    float2 texcoord : TEXCOORD;
};

VSOutput main(in SkeletalVertex input)
{
    
    // 頂点シェーダーでの利用例
    float4x4 boneMatrix = BonePalette[input.bone_index.x];
	// スキニング処理
    float4x4 skinnedMatrix = 0;
    skinnedMatrix += BonePalette[input.bone_index.x] * input.bone_weight.x;
    skinnedMatrix += BonePalette[input.bone_index.y] * input.bone_weight.y;
    skinnedMatrix += BonePalette[input.bone_index.z] * input.bone_weight.z;
    skinnedMatrix += BonePalette[input.bone_index.w] * input.bone_weight.w;
    
  //  float4x4 skinnedMatrix =
  //  {
  //      1.0f, 0.0f, 0.0f, 0.0f,
		//0.0f, 1.0f, 0.0f, 0.0f,
		//0.0f, 0.0f, 1.0f, 0.0f,
		//0.0f, 0.0f, 0.0f, 1.0f
  //  };
    
    VSOutput output = (VSOutput) 0;

    float4x4 mat = Camera.viewProjectionMatrix;
    float4 position = mul(skinnedMatrix, float4(input.position, 1.0f));
    float3 normal = input.normal;

    output.position = mul(mat, position);
    output.normal = normal;
    output.texcoord = input.texcoord;
    return output;
}