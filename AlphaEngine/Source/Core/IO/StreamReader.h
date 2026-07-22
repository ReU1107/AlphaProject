#pragma once
#include "Types.h"
#include "Container/Array.h"
#include "String/String.h"
#include "Math/Vector.h"

namespace Alpha
{
	enum class SeekPosition : uint8_t
	{
		Begin,	// 一番前
		Current,// 現在
		End		// 一番後ろ
	};

	class StreamReader
	{
	private:
		TArray<UInt8> m_raw_data{};
		USize m_cursor{ 0 };
	public:
		StreamReader() = default;

		void Open(StringView file_name);

		UInt8 ReadUInt8();
		UInt16 ReadUInt16();
		UInt32 ReadUInt32();

		Int8 ReadInt8();
		Int16 ReadInt16();
		Int32 ReadInt32();

		Vector2 ReadVector2();
		Vector3 ReadVector3();
		Vector4 ReadVector4();

		USize Read(void* destination, USize size);

		// ファイルサイズ取得
		USize GetFileSize() const;

		// ファイル位置変更(使わないし消すかも)
		void Seek(SeekPosition position, USize offset);
	};

}