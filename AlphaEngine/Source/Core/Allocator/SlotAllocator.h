#pragma once
#include "Core/Types.h"
#include "Core/Container/Array.h"

namespace Alpha
{
    class SlotAllocator
    {
    private:
        UInt32 m_max_num{ 0 };
        UInt32 m_allocated_num{ 0 };
        TArray<UInt32> m_free_indices{};

    public:
        void Initialize(UInt32 max_elements)
        {
            m_max_num = max_elements;
            m_allocated_num = 0;
            m_free_indices.clear();
        }

        bool Allocate(UInt32* out_index)
        {
            if (!m_free_indices.empty())
            {
                *out_index = m_free_indices.back();
                m_free_indices.pop_back();
                return true;
            }

            if (m_allocated_num >= m_max_num)
            {
                return false;
            }

            *out_index = m_allocated_num;
            m_allocated_num++;
            return true;
        }

        void Free(UInt32 index)
        {
            if (index >= m_allocated_num)
                return;

            m_free_indices.push_back(index);
        }

        UInt32 GetCurrentCapacity() const { return m_allocated_num; }

        void Reset()
        {
            m_allocated_num = 0;
            m_free_indices.clear();
        }
    };

}