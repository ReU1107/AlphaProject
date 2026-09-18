#ifndef ALPHA_PARTICLE_HLSLI
#define ALPHA_PARTICLE_HLSLI

struct ParticleData
{
    float3 position;
    float age;

    float3 velocity;
    float lifetime;

    uint packed_start_color;
    uint packed_end_color;

    float start_size;
    float end_size;

};

struct ParticleEmitCommand
{
    float4x4 emitter_world;

    float4 shape_params;

    float3 initial_velocity;
    uint spawn_count;

    float3 velocity_random;
    uint random_seed;

    float4 start_color;
    float4 end_color;

    float start_size;
    float end_size;
    float lifetime;
    uint source_shape;

    uint distribution;
    uint source_instance_index;
    uint material_index;
    uint flags;
};

struct ParticleBufferIndices
{
    uint read_particle;
    uint write_particle;

    uint emit_command;
    uint particle_counter;
    
    uint render_particle;
};

struct ParticleFrameConstants
{
    uint read_counter_index;
    uint write_counter_index;
    
    uint max_particle_count;
    uint emit_command_count;

    float time;
    float delta_time;
    uint step_count;
    
};

ConstantBuffer<ParticleBufferIndices> particle_buffers : register(b2);
ConstantBuffer<ParticleFrameConstants> particle_constants : register(b3);

static const uint PARTICLE_SHAPE_BOX = 0;
static const uint PARTICLE_SHAPE_SPHERE = 1;
static const uint PARTICLE_SHAPE_HEMISPHERE = 2;
static const uint PARTICLE_SHAPE_CONE = 3;
static const uint PARTICLE_SHAPE_CYLINDER = 4;
static const uint PARTICLE_SHAPE_POINT = 5;
static const uint PARTICLE_SHAPE_CIRCLE = 6;
static const uint PARTICLE_SHAPE_RECTANGLE = 7;
static const uint PARTICLE_SHAPE_LINE = 8;
static const uint PARTICLE_SHAPE_MESH = 9;
static const uint PARTICLE_SHAPE_SPLINE = 10;

// C++ EmitDistribution と同じ順番。
static const uint PARTICLE_DISTRIBUTION_CENTER = 0;
static const uint PARTICLE_DISTRIBUTION_VERTEX = 1;
static const uint PARTICLE_DISTRIBUTION_EDGE = 2;
static const uint PARTICLE_DISTRIBUTION_SURFACE = 3;
static const uint PARTICLE_DISTRIBUTION_VOLUME = 4;


#endif