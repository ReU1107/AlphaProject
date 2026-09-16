#include "Particle.hlsli"

ConstantBuffer<ParticleBufferIndices> buffer_indices : register(b2);
ConstantBuffer<ParticleFrameConstants> frame : register(b3);

[numthreads(1, 1, 1)]
void main()
{
    RWStructuredBuffer<uint> particle_counter = ResourceDescriptorHeap[buffer_indices.particle_counter];

    particle_counter[frame.write_counter_index] = 0;
}