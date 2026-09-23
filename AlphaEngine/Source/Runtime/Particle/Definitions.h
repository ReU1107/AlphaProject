#pragma once
#include "Core/Common.h"

namespace Alpha
{

    class CommandList;
    class TransientBufferAllocator;
}

namespace Alpha::Particle
{

    struct ParticleData
    {
        float3 position;
        float age;

        float3 velocity;
        float lifetime;

        float4 rotation;

        float3 angular_velocity;
        float inverse_mass;

        float3 acceleration;
        float drag;

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
        UInt32 spawn_count;

        float3 velocity_random;
        UInt32 random_seed;

        float4 start_color;
        float4 end_color;

        float start_size;
        float end_size;
        float lifetime;
        UInt32 source_shape;

        UInt32 distribution;
        UInt32 source_instance_index{ 0xFFFFFFFF };
        UInt32 material_index{ 0xFFFFFFFF };
        UInt32 flags{ 0 };
    };

    struct ParticleBufferIndices
    {
        UInt32 read_particle;
        UInt32 write_particle;

        UInt32 emit_command;
        UInt32 particle_counter;

        UInt32 render_particle;
    };

    struct ParticleUpdateConstants
    {
        UInt32 read_counter_index;
        UInt32 write_counter_index;

        UInt32 max_particle_count;
        UInt32 emit_command_count;

        float time;               // シミュレーションの経過秒
        float delta_time;         // 今回の更新時間
        UInt32 simulation_step;   // シミュレーションの更新回数
    };

    struct ParticleContext
    {
        CommandList* command_list{ nullptr };
        TransientBufferAllocator* transient_buffer{ nullptr };
        UInt32 current_frame_index{ 0 };

        float delta_time{ 0.0f };
    };
}