#pragma once
#include "Types.h"
#include "Container/Array.h"
#include "Math/Vector.h"

namespace Alpha
{
    class ByteWriter
    {
    private:
        TArray<UInt8> m_raw_data{};

    public:
        void WriteUInt8(UInt8 value) { Write(&value, sizeof(value)); }
        void WriteUInt16(UInt16 value) { Write(&value, sizeof(value)); }
        void WriteUInt32(UInt32 value) { Write(&value, sizeof(value)); }
        void WriteInt8(Int8 value) { Write(&value, sizeof(value)); }
        void WriteInt16(Int16 value) { Write(&value, sizeof(value)); }
        void WriteInt32(Int32 value) { Write(&value, sizeof(value)); }
        void WriteVector1(const float value) { Write(&value, sizeof(value)); }
        void WriteVector2(const Vector2& value) { Write(&value, sizeof(value)); }
        void WriteVector3(const Vector3& value) { Write(&value, sizeof(value)); }
        void WriteVector4(const Vector4& value) { Write(&value, sizeof(value)); }
        void WriteUVector1(const UVector1& value) { Write(&value, sizeof(value)); }
        void WriteUVector2(const UVector2& value) { Write(&value, sizeof(value)); }
        void WriteUVector3(const UVector3& value) { Write(&value, sizeof(value)); }
        void WriteUVector4(const UVector4& value) { Write(&value, sizeof(value)); }

        // パディング用
        void Align(size_t alignment)
        {
            size_t misaligned = m_raw_data.size() % alignment;
            if (misaligned > 0)
            {
                size_t padding = alignment - misaligned;
                m_raw_data.insert(m_raw_data.end(), padding, 0);
            }
        }

        void Write(const void* source, size_t size)
        {
            const UInt8* byte_ptr = reinterpret_cast<const UInt8*>(source);
            m_raw_data.insert(m_raw_data.end(), byte_ptr, byte_ptr + size);
        }

        void Reserve(size_t size)
        {
			m_raw_data.reserve(size);
        }

        const TArray<UInt8>& GetData() const { return m_raw_data; }
        void Clear() { m_raw_data.clear(); }

    };

}