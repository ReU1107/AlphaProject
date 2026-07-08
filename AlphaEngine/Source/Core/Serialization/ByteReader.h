#pragma once
#include "Types.h"
#include "Container/Array.h"
#include "Math/Vector.h"

namespace Alpha
{
    /*
    * IO側にあるBinaryReaderからbyte列を読み込む部分だけを切り出したクラス
    */

    class ByteReader
    {
    private:
        const UInt8* m_raw_data; // コピーを発生させず、ポインタで直接高速アクセス
        size_t m_size;
        size_t m_cursor;


    public:
        // 外部の std::vector をそのまま参照して読み込む場合
        ByteReader(const std::vector<UInt8>& data)
            : m_raw_data(data.data()), m_size(data.size()), m_cursor(0) 
        {
        }

        ByteReader(const UInt8* data, size_t size)
            : m_raw_data(data), m_size(size), m_cursor(0)
        {
        }

        UInt8  ReadUInt8() { UInt8  v; Read(&v, sizeof(v)); return v; }
        UInt16 ReadUInt16() { UInt16 v; Read(&v, sizeof(v)); return v; }
        UInt32 ReadUInt32() { UInt32 v; Read(&v, sizeof(v)); return v; }
        Int8   ReadInt8() { Int8   v; Read(&v, sizeof(v)); return v; }
        Int16  ReadInt16() { Int16  v; Read(&v, sizeof(v)); return v; }
        Int32  ReadInt32() { Int32  v; Read(&v, sizeof(v)); return v; }
        float  ReadFloat() { float v; Read(&v, sizeof(v)); return v; }

        Vector2 ReadVector2() { Vector2  v; Read(&v, sizeof(v)); return v; }
        Vector3 ReadVector3() { Vector3  v; Read(&v, sizeof(v)); return v; }
        Vector4 ReadVector4() { Vector4  v; Read(&v, sizeof(v)); return v; }

        // 🌟 アライメント分だけカーソルを飛ばす関数
        void Align(size_t alignment)
        {
            size_t misaligned = m_cursor % alignment;
            if (misaligned > 0)
            {
                m_cursor += (alignment - misaligned);
            }
        }

        bool Read(void* destination, size_t size)
        {
            // 元のコードの安全性をキープ（バッファオーバーラン防止）
            if (m_cursor + size > m_size)
                return false;

            std::memcpy(destination, &m_raw_data[m_cursor], size);
            m_cursor += size;
            return true;
        }

        size_t GetCursor() const { return m_cursor; }
        bool IsEOF() const { return m_cursor >= m_size; }


    };
}