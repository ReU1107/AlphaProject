#include "Resource.hlsli"

struct [raypayload] RayPayload
{
    float3 color : write(miss, closesthit) : read(caller);
};

SamplerState sam : register(s0);

ConstantBuffer<SystemBufferIndices> buffer_indices : register(b0);



[shader("closesthit")]
void MyClosestHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[buffer_indices.instance];
    
    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[buffer_indices.mesh];
    //StructuredBuffer<uint> index_buffer = ResourceDescriptorHeap[buffer_indices.in];
    ByteAddressBuffer attribute_buffer = ResourceDescriptorHeap[buffer_indices.attribute];

    StructuredBuffer<MaterialData> material_buffer = ResourceDescriptorHeap[buffer_indices.material];
    
    ByteAddressBuffer material_table_buffer = ResourceDescriptorHeap[buffer_indices.material_table];
        
    uint instance_id = InstanceID();
    
    InstanceData instance = instance_buffer[instance_id];
    
    MeshData mesh = mesh_buffer[instance.mesh_index];
    
    uint vertId = PrimitiveIndex() * 3;
    
    uint geometry_index = GeometryIndex();
    
    uint material_offset = material_table_buffer.Load(instance.material_table_index);
    
    MaterialData material = material_buffer[material_offset + geometry_index];
    

    
    float4 final_base = float4(material.props.base_color, material.props.opacity);

    
        
    // 衝突時の色（テスト用に赤)
    payload.color = final_base.xyz;
}