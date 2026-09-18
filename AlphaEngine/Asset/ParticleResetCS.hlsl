#include "Particle.hlsli"


[numthreads(1, 1, 1)]
void main()
{
    RWStructuredBuffer<uint> particle_counter = ResourceDescriptorHeap[particle_buffers.particle_counter];

    particle_counter[particle_constants.write_counter_index] = 0;
}