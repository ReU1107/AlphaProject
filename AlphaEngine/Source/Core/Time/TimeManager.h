#pragma once
#include "Core/Subsystem.h"

namespace Alpha 
{

    class TimeManager : public Subsystem<TimeManager> 
    {
    private:
        float m_delta_time{ 0.0f };
        float m_elapsed_time{ 0.0f };
        uint64_t m_frame_count{ 0 };

    public:
        struct Args {};

        bool Initialize(const Args& args);
        void Finalize();

        void Update(float delta);

        float GetDeltaTime() const { return m_delta_time; }
        float GetElapsedTime() const { return m_elapsed_time; }
        uint64_t GetFrameCount() const { return m_frame_count; }
        float GetFPS() const { return (m_delta_time > 0) ? 1.0f / m_delta_time : 0.0f; }
    };

}