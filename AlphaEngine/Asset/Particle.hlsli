
struct ParticleData
{
    float3 position;
    float age;

    float3 velocity;
    float lifetime;

    float3 size;
    
    uint packed_color;
    uint random_seed;
};

struct ParticleEmitCommand
{
    float3 world_position;
    float delta_time;

    float3 initial_velocity;
    float emission_rate;

    float4 start_color;
    float4 end_color;

    float start_size;
    float end_size;
    float lifetime;
    uint spawn_count;

    uint material_index;
    uint flags;
    uint target_buffer_index;
    uint padding;
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

    float delta_time;
};

