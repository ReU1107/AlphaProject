#include "Particle.hlsli"

ConstantBuffer<ParticleBufferIndices> buffer_indices : register(b0);
ConstantBuffer<ParticleFrameContext> frame : register(b1);

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    StructuredBuffer<ParticleEmitCommand> emit_commands = ResourceDescriptorHeap[buffer_indices.emit_command];
    RWStructuredBuffer<ParticleData> particle_buffer = ResourceDescriptorHeap[buffer_indices.write_particle];
    RWStructuredBuffer<uint> particle_counter = ResourceDescriptorHeap[buffer_indices.particle_counter];
    
    
    const uint emitter_index = id.x;

    if (emitter_index >= frame.emit_command_count)
        return;
    
    ParticleEmitCommand emit_command = emit_commands[emitter_index];
    
    uint particle_offset = 0;
    InterlockedAdd(particle_counter[frame.write_counter_index], emit_command.spawn_count, particle_offset);
    
    if (particle_offset >= frame.max_particle_count)
        return;
    
    const uint spawnable_count = min(emit_command.spawn_count, frame.max_particle_count - particle_offset);
    

    for (uint i = 0; i < spawnable_count; ++i)
    {
        ParticleData particle = (ParticleData) 0;
        particle.position = emit_command.world_position;
        particle.lifetime = emit_command.lifetime;
        
        
        
        particle_buffer[particle_offset + i] = particle;
    }
    
    
}