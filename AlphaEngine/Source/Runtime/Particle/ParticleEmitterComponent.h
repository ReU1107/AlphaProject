#pragma once
#include "Core/Common.h"

namespace Alpha::Particle
{
    enum class SourceShape
    {
        Box,        
        Sphere,     
        Hemisphere, 
        Cone,       
        Cylinder,   

        Point,      
        Circle,     
        Rectangle,  
        Line,       

        Mesh,       
        Spline,     
    };

    enum class EmitDistribution
    {
        Center,  
        Vertex,  
        Edge,    
        Surface, 
        Volume,  
    };

    struct ParticleEmitterComponent
    {
        UInt32 max_particle_count{ 1000 };

        float emission_rate{ 100.0f };
        float lifetime{ 5.0f };             // 生成したパーティクル1つの生存時間

        float duration{ 1.0f };             // このエミッターが再生開始されてから

        float3 initial_velocity{ 0.0f };
        float3 velocity_random{ 0.0f };

        float4 start_color = float4::One;
        float4 end_color = float4::Zero;

        float start_size{ 0.1f };
        float end_size{ 0.0f };

        UInt32 random_seed{ 0 };

        SourceShape source_shape{ 0 };
        EmitDistribution distribution{};

        float spawn_accumulator{ 0.0f };
        float3 box_half_size{ 0.5f, 0.5f, 0.5f };
        float radius{ 0.5f };
        float height{ 1.0f };
        float2 rectangle_half_size{ 0.5f, 0.5f };
        float line_length{ 1.0f };

        bool emitting{ true };

        void Start()
        {
            emitting = true;
        }

        void Stop()
        {
            emitting = false;
        }
    };
}