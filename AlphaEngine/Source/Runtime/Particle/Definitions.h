#pragma once
#include "Core/Common.h"

namespace Alpha
{

    struct ParticleData
    {
        float3 position;
        float age;

        float3 velocity;
        float lifetime;

        UInt32 packed_color;
        UInt32 packed_size_rotation;
        UInt32 random_seed;
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
        UInt32 spawn_count;

        UInt32 material_index;
        UInt32 flags;
        UInt32 padding;
    };

    class CommandList;
    class TransientBufferAllocator;

    struct ParticleContext
    {
        CommandList* command_list{ nullptr };
        TransientBufferAllocator* transient_buffer{ nullptr };
        UInt32 current_frame_index{ 0 };

        UInt64 current_camera_offset{ 0 };

        float delta_time{ 0.0f };
    };
}