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

// --- 0D / 1D ---
static const uint PARTICLE_SHAPE_POINT                      = 0;
static const uint PARTICLE_SHAPE_LINE                       = 1;

// --- 2D ---
static const uint PARTICLE_SHAPE_TRIANGLE                   = 2;
static const uint PARTICLE_SHAPE_RECTANGLE                  = 3;
static const uint PARTICLE_SHAPE_CIRCLE                     = 4;
static const uint PARTICLE_SHAPE_ELLIPSE                    = 5;
static const uint PARTICLE_SHAPE_ANNULUS                    = 6; // 円環
static const uint PARTICLE_SHAPE_ARC                        = 7; // 円弧
static const uint PARTICLE_SHAPE_SECTOR                     = 8; // 扇形
static const uint PARTICLE_SHAPE_REGULAR_POLYGON            = 9; // 正多角形

// --- 3D ---
static const uint PARTICLE_SHAPE_BOX                        = 10;
static const uint PARTICLE_SHAPE_ROUNDED_BOX                = 11;
static const uint PARTICLE_SHAPE_SPHERE                     = 12;
static const uint PARTICLE_SHAPE_HEMISPHERE                 = 13;
static const uint PARTICLE_SHAPE_ELLIPSOID                  = 14;
static const uint PARTICLE_SHAPE_CONE                       = 15;
static const uint PARTICLE_SHAPE_CYLINDER                   = 16;
static const uint PARTICLE_SHAPE_TRIANGULAR_PYRAMID         = 17; // 三角錐
static const uint PARTICLE_SHAPE_PYRAMID                    = 18; // 四角錐
static const uint PARTICLE_SHAPE_REGULAR_PYRAMID            = 19; // 正多角錐
static const uint PARTICLE_SHAPE_TRIANGULAR_PRISM           = 20; // 三角柱
static const uint PARTICLE_SHAPE_REGULAR_PRISM              = 21; // 正多角柱
static const uint PARTICLE_SHAPE_CAPSULE                    = 22;
static const uint PARTICLE_SHAPE_TORUS                      = 23;
static const uint PARTICLE_SHAPE_TUBE                       = 24; // 中空円柱
static const uint PARTICLE_SHAPE_CONE_FRUSTUM               = 25; // 円錐台
static const uint PARTICLE_SHAPE_PYRAMID_FRUSTUM            = 26; // 四角錐台
static const uint PARTICLE_SHAPE_REGULAR_PYRAMID_FRUSTUM    = 27; // 正多角錐台

// --- Special ---
static const uint PARTICLE_SHAPE_MESH = 28;
static const uint PARTICLE_SHAPE_SPLINE = 29;

// C++ EmitDistribution と同じ順番。
static const uint PARTICLE_DISTRIBUTION_CENTER = 0;
static const uint PARTICLE_DISTRIBUTION_VERTEX = 1;
static const uint PARTICLE_DISTRIBUTION_EDGE = 2;
static const uint PARTICLE_DISTRIBUTION_SURFACE = 3;
static const uint PARTICLE_DISTRIBUTION_VOLUME = 4;


#endif