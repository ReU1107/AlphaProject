#include "Resource.hlsli"
#include "Lighting.hlsli"

struct PassConstants
{
    uint result_index;
    uint visibility;
    uint depth;
};

ConstantBuffer<PassConstants> pass_info : register(b2);

float3 DebugColor(uint id1, uint id2)
{
    // MurmurHash3のミキシング（ハッシュ結合）の手法をベースにした実装
    uint h = id1;
    
    // 2つ目のIDを混ぜ込む
    h ^= id2;
    h *= 0xcc9e2d51;
    h = (h << 15) | (h >> 17);
    h = h * 5 + 0xe6546b64;

    // 元のハッシュ関数のファイナライズ処理
    h ^= h >> 16;
    h *= 0x7feb352d;
    h ^= h >> 15;
    h *= 0x846ca68b;
    h ^= h >> 16;

    return float3(
        float((h >> 0) & 0xff) / 255.0,
        float((h >> 8) & 0xff) / 255.0,
        float((h >> 16) & 0xff) / 255.0
    );
}

uint GetUpper16(uint value)
{
    return value >> 16;
}

// 後半（下位）16bitを抽出する関数
uint GetLower16(uint value)
{
    return value & 0xFFFF;
}

[numthreads(8, 8, 1)]
void main(uint3 dispatch_index : SV_DispatchThreadID)
{
    RWTexture2D<float4> result = ResourceDescriptorHeap[pass_info.result_index];
    //Texture2D<float4> visibility_tex = ResourceDescriptorHeap[pass_info.visibility];
    Texture2D<uint2> visibility_tex = ResourceDescriptorHeap[pass_info.visibility];
    Texture2D<float> depth_tex = ResourceDescriptorHeap[pass_info.depth];
    
    
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[render_buffers.instance];
    StructuredBuffer<float3> position_buffer = ResourceDescriptorHeap[render_buffers.current_position_srv];
    
    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[render_buffers.mesh];
    
    StructuredBuffer<Meshlet> meshlet_buffer = ResourceDescriptorHeap[render_buffers.meshlet];
    
    StructuredBuffer<uint> vertex_indices_buffer = ResourceDescriptorHeap[render_buffers.vertex_indices];
    StructuredBuffer<uint> triangle_indices_buffer = ResourceDescriptorHeap[render_buffers.triangle_indices];
    
    ByteAddressBuffer attribute_buffer = ResourceDescriptorHeap[render_buffers.attribute];

    StructuredBuffer<MaterialData> material_buffer = ResourceDescriptorHeap[render_buffers.material];
    
    StructuredBuffer<uint> material_table_buffer = ResourceDescriptorHeap[render_buffers.material_table];
    
    StructuredBuffer<MaterialData> override_material_buffer = ResourceDescriptorHeap[render_buffers.override_material];
    
    SamplerState sam = SamplerDescriptorHeap[SAMPLER_PRESET_LINEAR_CLAMP];

    uint2 index = dispatch_index.xy;

    uint2 thread_dimension;
    result.GetDimensions(thread_dimension.x, thread_dimension.y);

    float2 uv = ((float2) dispatch_index + 0.5f) / thread_dimension;
    
    uint2 visibility = visibility_tex.Load(int3(index, 0));
    float depth = depth_tex.Load(int3(index, 0));
    
    uint instance_id = visibility.x;
    
    InstanceData instance = instance_buffer[instance_id];
    
    
    MeshData mesh = mesh_buffer[instance.mesh_index];

    uint local_meshlet_id = GetUpper16(visibility.g);
    uint primitive_id = GetLower16(visibility.g);
    
    uint meshlet_id = mesh.meshlet_element_offset + local_meshlet_id;

    Meshlet meshlet = meshlet_buffer[meshlet_id];

    uint triangle_index_offset = mesh.triangle_element_offset + meshlet.triangle_element_offset;
    
    // meshlet内のプリミティブIDから、元の三角形インデックスを取得
    uint triangle_index = triangle_indices_buffer[triangle_index_offset + primitive_id];
        
    uint idx0 = (triangle_index >> 0) & 0xFFu;
    uint idx1 = (triangle_index >> 8) & 0xFFu;
    uint idx2 = (triangle_index >> 16) & 0xFFu;
    
    uint vertex_index_offset = mesh.vertex_element_offset + meshlet.vertex_element_offset;

    // 元のモデルの頂点インデックスを取得
    uint v0 = vertex_indices_buffer[vertex_index_offset + idx0];
    uint v1 = vertex_indices_buffer[vertex_index_offset + idx1];
    uint v2 = vertex_indices_buffer[vertex_index_offset + idx2];
    
    float3 w0 = mul(instance.world_matrix, float4(position_buffer[instance.vertex_element_offset + v0], 1.0f));
    float3 w1 = mul(instance.world_matrix, float4(position_buffer[instance.vertex_element_offset + v1], 1.0f));
    float3 w2 = mul(instance.world_matrix, float4(position_buffer[instance.vertex_element_offset + v2], 1.0f));
        
    
    float3 world_position = ScreenToWorld(uv, depth, render_frame.inv_view_proj);
    
    float3 barycentrics = ComputeBarycentric(w0, w1, w2, world_position);
    
    
    uint normal_offset = 0;
    uint bone_offset = normal_offset + ((mesh.attribute_flags & VERTEX_ATTR_NORMALS) ? 12 : 0);
    uint uv_offset = bone_offset + ((mesh.attribute_flags & VERTEX_ATTR_BONE_WEIGHTS) ? 32 : 0);

    float3 normal = float3(0.0f, 1.0f, 0.0f);
    float2 mesh_uv = float2(0.0f, 0.0f);
    
    // 重心座標による補間
    float w = barycentrics.x;
    float u = barycentrics.y;
    float v = barycentrics.z;
    
    {
        uint addr0 = mesh.vertex_attribute_byte_offset + (v0 * mesh.vertex_stride) + normal_offset;
        uint addr1 = mesh.vertex_attribute_byte_offset + (v1 * mesh.vertex_stride) + normal_offset;
        uint addr2 = mesh.vertex_attribute_byte_offset + (v2 * mesh.vertex_stride) + normal_offset;
        normal = (asfloat(attribute_buffer.Load3(addr0)) * w) + (asfloat(attribute_buffer.Load3(addr1)) * u) + (asfloat(attribute_buffer.Load3(addr2)) * v);
        
        normal = normalize(normal);
    }
    
    {
        uint addr0 = mesh.vertex_attribute_byte_offset + (v0 * mesh.vertex_stride) + uv_offset;
        uint addr1 = mesh.vertex_attribute_byte_offset + (v1 * mesh.vertex_stride) + uv_offset;
        uint addr2 = mesh.vertex_attribute_byte_offset + (v2 * mesh.vertex_stride) + uv_offset;
        mesh_uv = (asfloat(attribute_buffer.Load2(addr0)) * w) + (asfloat(attribute_buffer.Load2(addr1)) * u) + (asfloat(attribute_buffer.Load2(addr2)) * v);
    }
    
    uint material_index = material_table_buffer[mesh.material_table_index + meshlet.material_index];
    
    MaterialData material = material_buffer[material_index];
    
        
    Texture2D base_tex = ResourceDescriptorHeap[material.base_color.texture_index];

    SamplerState base_sampler = SamplerDescriptorHeap[material.base_color.sampler_index];
    
    float4 base_color_sample = base_tex.Sample(base_sampler, mesh_uv);

    
    //float3 color = DebugColor(visibility.x, visibility.y);
    float3 color = base_color_sample.rgb;
    
    
    //float3 color = visibility_tex.Sample(sam, uv).rgb;

    result[index] = float4(color, 1.0f);
}

/*
テスト用visibility_texに色をいれてそのまま表示するだけの
[numthreads(8, 8, 1)]
void main(uint3 dispatch_index : SV_DispatchThreadID)
{
    RWTexture2D<float4> result = ResourceDescriptorHeap[pass_info.result_index];
    Texture2D<float4> visibility_tex = ResourceDescriptorHeap[pass_info.visibility];
    Texture2D<float> depth_tex = ResourceDescriptorHeap[pass_info.depth];
    
    SamplerState sam = SamplerDescriptorHeap[SAMPLER_PRESET_LINEAR_CLAMP];

    uint2 index = dispatch_index.xy;

    uint2 thread_dimension;
    result.GetDimensions(thread_dimension.x, thread_dimension.y);

    float2 uv = ((float2) dispatch_index + 0.5f) / thread_dimension;
    
    float4 visibility = visibility_tex.Sample(sam, uv).rgb;
    
    
    float3 color = visibility_tex.Sample(sam, uv).rgb;

    result[index] = float4(color, 1.0f);
}

*/