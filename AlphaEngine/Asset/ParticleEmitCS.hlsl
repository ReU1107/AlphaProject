#include "Resource.hlsli"
#include "Particle.hlsli"
#include "Sample.hlsli"

uint packUnorm4x8(float4 val)
{
    uint4 u = uint4(round(clamp(val, 0.0, 1.0) * 255.0));
    return (u.x | (u.y << 8) | (u.z << 16) | (u.w << 24));
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    StructuredBuffer<ParticleEmitCommand> emit_commands = ResourceDescriptorHeap[particle_buffers.emit_command];
    RWStructuredBuffer<ParticleData> particle_buffer = ResourceDescriptorHeap[particle_buffers.write_particle];
    RWStructuredBuffer<uint> particle_counter = ResourceDescriptorHeap[particle_buffers.particle_counter];
    StructuredBuffer<InstanceData> instance_buffer = ResourceDescriptorHeap[render_buffers.instance];

    StructuredBuffer<float3> position_buffer = ResourceDescriptorHeap[render_buffers.current_position_srv];
    StructuredBuffer<uint> index_buffer = ResourceDescriptorHeap[render_buffers.index];

    StructuredBuffer<MeshData> mesh_buffer = ResourceDescriptorHeap[render_buffers.mesh];
    ByteAddressBuffer attribute_buffer = ResourceDescriptorHeap[render_buffers.attribute];
    
    const uint emitter_index = id.x;

    if (emitter_index >= particle_constants.emit_command_count)
        return;
    
    ParticleEmitCommand emit_command = emit_commands[emitter_index];

    // このエミッターが生成するパーティクル数をカウンターに加算して、現存するパーティクル数を取得
    uint particle_offset = 0;
    InterlockedAdd(particle_counter[particle_constants.write_counter_index], emit_command.spawn_count, particle_offset);
    
    
    // 最大数から制限
    if (particle_offset >= particle_constants.max_particle_count)
        return;

    uint spawnable_count = min(emit_command.spawn_count, particle_constants.max_particle_count - particle_offset);

    InstanceData instance = (InstanceData) 0;
    MeshData mesh = (MeshData) 0;
    
    for (uint i = 0; i < spawnable_count; ++i)
    {
        ParticleData particle = (ParticleData) 0;

        uint unique_id = i ^ (emitter_index * 131071u) ^ (asuint(particle_constants.step_count) * 534521u) ^ emit_command.random_seed;
        
        uint seed = StepHashUint(unique_id);
        
        float3 local_position = float3(0, 0, 0);

        switch (emit_command.source_shape)
        {
            case PARTICLE_SHAPE_BOX:
                if (emit_command.distribution == PARTICLE_DISTRIBUTION_VERTEX)
                    local_position = SampleBoxVertex(emit_command.shape_params.xyz, seed);
                else if (emit_command.distribution == PARTICLE_DISTRIBUTION_EDGE)
                    local_position = SampleBoxEdge(emit_command.shape_params.xyz, seed);
                else if (emit_command.distribution == PARTICLE_DISTRIBUTION_VOLUME)
                    local_position = SampleBoxVolume(emit_command.shape_params.xyz, seed);
                else
                    local_position = SampleBoxSurface(emit_command.shape_params.xyz, seed);
                break;

            case PARTICLE_SHAPE_SPHERE:
                if (emit_command.distribution == PARTICLE_DISTRIBUTION_VOLUME)
                    local_position = SampleSphereVolume(emit_command.shape_params.x, seed);
                else
                    local_position = SampleSphereSurface(emit_command.shape_params.x, seed);
                break;

            case PARTICLE_SHAPE_HEMISPHERE:
                if (emit_command.distribution == PARTICLE_DISTRIBUTION_VOLUME)
                    local_position = SampleHemisphereVolume(emit_command.shape_params.x, seed);
                else
                    local_position = SampleHemisphereSurface(emit_command.shape_params.x, seed);
                break;

            case PARTICLE_SHAPE_CONE:
                if (emit_command.distribution == PARTICLE_DISTRIBUTION_VOLUME)
                    local_position = SampleConeVolume(emit_command.shape_params.x, emit_command.shape_params.y, seed);
                else
                    local_position = SampleConeSurface(emit_command.shape_params.x, emit_command.shape_params.y, seed);
                break;

            case PARTICLE_SHAPE_CYLINDER:
                if (emit_command.distribution == PARTICLE_DISTRIBUTION_EDGE)
                    local_position = SampleCylinderEdge(emit_command.shape_params.x, emit_command.shape_params.y, seed);
                else if (emit_command.distribution == PARTICLE_DISTRIBUTION_VOLUME)
                    local_position = SampleCylinderVolume(emit_command.shape_params.x, emit_command.shape_params.y, seed);
                else
                    local_position = SampleCylinderSurface(emit_command.shape_params.x, emit_command.shape_params.y, seed);
                break;

            case PARTICLE_SHAPE_POINT:
                local_position = float3(0.0f, 0.0f, 0.0f);
                break;

            case PARTICLE_SHAPE_CIRCLE:
                if (emit_command.distribution == PARTICLE_DISTRIBUTION_EDGE)
                    local_position = SampleCircleEdge(emit_command.shape_params.x, seed);
                else
                    local_position = SampleCircleSurface(emit_command.shape_params.x, seed);
                break;

            case PARTICLE_SHAPE_RECTANGLE:
                if (emit_command.distribution == PARTICLE_DISTRIBUTION_VERTEX)
                    local_position = SampleRectangleVertex(emit_command.shape_params.xy, seed);
                else if (emit_command.distribution == PARTICLE_DISTRIBUTION_EDGE)
                    local_position = SampleRectangleEdge(emit_command.shape_params.xy, seed);
                else
                    local_position = SampleRectangleSurface(emit_command.shape_params.xy, seed);
                break;

            case PARTICLE_SHAPE_LINE:
                if (emit_command.distribution == PARTICLE_DISTRIBUTION_VERTEX)
                    local_position = SampleLineVertex(emit_command.shape_params.x, seed);
                else
                    local_position = SampleLineEdge(emit_command.shape_params.x, seed);
                break;

            case PARTICLE_SHAPE_MESH:
                instance = instance_buffer[emit_command.source_instance_index];
                mesh = mesh_buffer[instance.mesh_index];
    
                if (emit_command.distribution == PARTICLE_DISTRIBUTION_VERTEX)
                    local_position = SampleMeshVertex(position_buffer, index_buffer, instance.vertex_element_offset, mesh.index_element_offset, mesh.index_count, seed);
                else
                    local_position = SampleMeshSurface(position_buffer, index_buffer, instance.vertex_element_offset, mesh.index_element_offset, mesh.index_count, seed);
                break;

            default:
                local_position = float3(0.0f, 0.0f, 0.0f);
                break;
        }

        particle.position = mul(emit_command.emitter_world, float4(local_position, 1.0f)).xyz;

        float3 emission_direction = normalize(local_position);
        particle.velocity = emit_command.initial_velocity + (emission_direction);
        particle.drag = 0;
        particle.lifetime = emit_command.lifetime;
        particle.start_size = emit_command.start_size;
        particle.end_size = emit_command.end_size;
        particle.packed_start_color = packUnorm4x8(emit_command.start_color);
        particle.packed_end_color = packUnorm4x8(emit_command.end_color);

        particle_buffer[particle_offset + i] = particle;
    }
    
}