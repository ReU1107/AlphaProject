#include "Particle.hlsli"
#include "ExecuteIndirect.hlsli"

ConstantBuffer<ParticleBufferIndices> buffer_indices : register(b0);
ConstantBuffer<ParticleFrameContext> frame : register(b1);


[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    RWStructuredBuffer<uint> particle_counter               = ResourceDescriptorHeap[buffer_indices.particle_counter];
    RWStructuredBuffer<DispatchMeshArgs> dispatch_arguments = ResourceDescriptorHeap[buffer_indices.dispatch_arguments];

    uint particle_count = min(particle_counter[frame.write_counter_index], frame.max_particle_count);

    DispatchMeshArgs args;
    args.thread_group_count_x = particle_count;
    args.thread_group_count_y = 1;
    args.thread_group_count_z = 1;

    dispatch_arguments[0] = args;
    
    
}