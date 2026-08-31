#include "Particle.hlsli"

ConstantBuffer<ParticleBufferIndices> buffer_indices : register(b0);
ConstantBuffer<ParticleFrameContext> frame : register(b1);

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    StructuredBuffer<ParticleData> read_particle_buffer = ResourceDescriptorHeap[buffer_indices.read_particle];
    RWStructuredBuffer<ParticleData> write_particle_buffer = ResourceDescriptorHeap[buffer_indices.write_particle];
    RWStructuredBuffer<uint> particle_counter = ResourceDescriptorHeap[buffer_indices.particle_counter];
        
    const uint alive_count = particle_counter[frame.read_counter_index];

    const uint particle_index = id.x;

    if (particle_index >= alive_count)
        return;
    
    // 現存するパーティクルの更新
    ParticleData particle = read_particle_buffer[particle_index];
    
    particle.lifetime -= frame.delta_time;
    
    if (particle.lifetime <= 0.0f)
        return;

    // particleの属性を見ながら更新

    // 更新しても生存しているならもう一度書き込む
    
    uint write_index;
    InterlockedAdd(particle_counter[frame.write_counter_index], 1, write_index);
    
    write_particle_buffer[write_index] = particle;
    
}