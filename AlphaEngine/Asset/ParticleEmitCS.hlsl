#include "Particle.hlsli"

ConstantBuffer<ParticleBufferIndices> buffer_indices : register(b2);
ConstantBuffer<ParticleFrameConstants> frame_constants : register(b3);

uint packUnorm4x8(float4 val)
{
    uint4 u = uint4(round(clamp(val, 0.0, 1.0) * 255.0));
    return (u.x | (u.y << 8) | (u.z << 16) | (u.w << 24));
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    StructuredBuffer<ParticleEmitCommand> emit_commands = ResourceDescriptorHeap[buffer_indices.emit_command];
    RWStructuredBuffer<ParticleData> particle_buffer = ResourceDescriptorHeap[buffer_indices.write_particle];
    RWStructuredBuffer<uint> particle_counter = ResourceDescriptorHeap[buffer_indices.particle_counter];
    
    const uint emitter_index = id.x;

    if (emitter_index >= frame_constants.emit_command_count)
        return;
    
    ParticleEmitCommand emit_command = emit_commands[emitter_index];

    // このエミッターが生成するパーティクル数をカウンターに加算して、現存するパーティクル数を取得
    uint particle_offset = 0;
    InterlockedAdd(particle_counter[frame_constants.write_counter_index], emit_command.spawn_count, particle_offset);
    

    // 最大数から制限
    if (particle_offset >= frame_constants.max_particle_count)
        return;

    uint spawnable_count = min(emit_command.spawn_count, frame_constants.max_particle_count - particle_offset);

    for (uint i = 0; i < spawnable_count; ++i)
    {
        ParticleData particle = (ParticleData) 0;

        particle.position = emit_command.world_position;
        particle.velocity = emit_command.initial_velocity;
        particle.lifetime = emit_command.lifetime;

        float size = emit_command.start_size;
        particle.size = float3(size, size, size);
        particle.packed_color = packUnorm4x8(emit_command.start_color);

        particle.random_seed = asuint(emit_command.lifetime);

        particle_buffer[particle_offset + i] = particle;
    }
    
}