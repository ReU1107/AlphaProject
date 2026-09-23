#include "Particle.hlsli"
#include "Sample.hlsli"

static const float3 Gravity = float3(0.0f, -9.8f, 0.0f);

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    StructuredBuffer<ParticleData> read_particle_buffer = ResourceDescriptorHeap[particle_buffers.read_particle];
    RWStructuredBuffer<ParticleData> write_particle_buffer = ResourceDescriptorHeap[particle_buffers.write_particle];
    RWStructuredBuffer<uint> particle_counter = ResourceDescriptorHeap[particle_buffers.particle_counter];
        
    const uint alive_count = min(particle_counter[particle_constants.read_counter_index], particle_constants.max_particle_count);

    const uint particle_index = id.x;

    if (particle_index >= alive_count)
        return;
    
    const float delta_time = particle_constants.delta_time;
    
    // 現存するパーティクルの更新
    ParticleData particle = read_particle_buffer[particle_index];

    particle.age += delta_time;

    if (particle.age >= particle.lifetime)
    {
        return;
    }


    particle.velocity *= exp(-particle.drag * delta_time);

   // particle.velocity += Gravity * delta_time;

    particle.position += particle.velocity * delta_time;
    
    
    uint write_index;
    InterlockedAdd(particle_counter[particle_constants.write_counter_index], 1, write_index);
    
    write_particle_buffer[write_index] = particle;
    
}