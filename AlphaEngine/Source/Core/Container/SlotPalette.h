#pragma once
#include "Core/Types.h"
#include "Core/Container/Array.h"
#include "Core/Container/Map.h" 
#include "Core/Allocator/SlotAllocator.h"
#include "Core/Assertion.h"

namespace Alpha
{
    // Tの実体オブジェクト配列をハッシュとindexで監理するクラス
    // 実体を先に作っておきそこから配布する感じの作りだから動的変化はしないつもりではいる
    template <typename T>
    class TSlotPalette
    {
    private:
        TMap<UInt64, UInt32> m_key_to_index_map{};
        TArray<T> m_palette{};
        SlotAllocator m_allocator{};

    public:

        void Initialize(UInt32 max_element_count)
        {
            m_palette.resize(max_element_count);

            m_allocator.Initialize(max_element_count);
        }

        bool Contains(UInt64 key) const
        {
            return m_key_to_index_map.contains(key);
        }

        bool TryGetIndex(UInt64 key, UInt32* out_index) const
        {
            auto found = m_key_to_index_map.find(key);
            if (found != m_key_to_index_map.cend())
            {
                if (out_index) *out_index = found->second;
                return true;
            }
            return false;
        }

        T* Find(UInt64 key)
        {
            auto found = m_key_to_index_map.find(key);
            if (found != m_key_to_index_map.cend())
                return &m_palette[found->second];
            return nullptr;
        }

        T& Allocate(UInt64 key, UInt32& out_index)
        {
            auto found = m_key_to_index_map.find(key);
            if (found != m_key_to_index_map.cend())
            {
                ALPHA_LOG_WARN(L"すでに登録されているキーでAllocateしようとしました: %llu", key);
                out_index = found->second;
                return m_palette[out_index];
            }

            UInt32 index = 0;
            if (!m_allocator.Allocate(&index))
            {
                ALPHA_LOG_WARN(L"想定した量を超えています初期化時の数を増やすなどし対処してください");
                // 動的拡張などのフォールバック処理...
            }

            m_key_to_index_map[key] = index;
            out_index = index;

            return m_palette[index];
        }

        void Free(UInt64 key)
        {
            if (!m_key_to_index_map.contains(key))
            {
                ALPHA_LOG_WARN(L"登録されていないキーを解放しようとしました: %llu", key);
                return;
            }

            UInt32 index = m_key_to_index_map[key];
            m_allocator.Free(index);
            m_palette[index] = T{};
            m_key_to_index_map.erase(key);
        }

        T& operator[](UInt32 index)
        {
            return m_palette[index];
        }

        const T& operator[](UInt32 index) const
        {
            return m_palette[index];
        }

        T& GetAt(UInt32 index) { return (*this)[index]; }
        const T& GetAt(UInt32 index) const { return (*this)[index]; }
    };
}