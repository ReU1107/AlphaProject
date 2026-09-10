#pragma once
#include "Core/Common.h"

namespace Alpha
{
    struct ParticleEmitterComponent
    {
        UInt32 max_particle_count{ 1000 };

        float emission_rate{ 100.0f };
        float lifetime{ 5.0f };

        float3 initial_velocity{ 0.0f };
        float3 velocity_random{ 0.0f };

        float4 start_color = float4::One;
        float4 end_color = float4::Zero;

        float start_size{ 0.1f };
        float end_size{ 0.0f };

        float spawn_accumulator{ 0.0f };  

        UInt32 source_model{ 0 };   // どの形状でサンプリングするか

    };
}