#include "Particle.hlsli"
#include "ExecuteIndirect.hlsli"

struct PrerenderPassConstants
{
    uint dispatch_arguments;

};

ConstantBuffer<ParticleBufferIndices> buffer_indices : register(b2);
ConstantBuffer<ParticleFrameConstants> frame_constants : register(b3);
ConstantBuffer<PrerenderPassConstants> pass_constants : register(b4);


[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    RWStructuredBuffer<uint> particle_counter               = ResourceDescriptorHeap[buffer_indices.particle_counter];
    RWStructuredBuffer<DispatchMeshArgs> dispatch_arguments = ResourceDescriptorHeap[pass_constants.dispatch_arguments];

    uint particle_count = min(particle_counter[frame_constants.write_counter_index], frame_constants.max_particle_count);

    DispatchMeshArgs args;
    args.thread_group_count_x = particle_count;
    args.thread_group_count_y = 1;
    args.thread_group_count_z = 1;

    dispatch_arguments[0] = args;
    
    
}