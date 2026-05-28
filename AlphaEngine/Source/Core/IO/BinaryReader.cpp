#include "BinaryReader.h"
#include <fstream>
#include <filesystem>

namespace Alpha
{
	void BinaryReader::Open(StringView file_name)
	{
		std::ifstream stream(std::filesystem::path(file_name.c_str()), std::ios::binary | std::ios::ate);
		if (!stream.fail())
		{
			auto size = stream.tellg();
			m_raw_data.resize(size);
			stream.seekg(0, std::ios::beg);
			stream.read(reinterpret_cast<char*>(m_raw_data.data()), size);
		}
	}

	uint8_t BinaryReader::ReadUInt8()
	{
		uint8_t value;
		Read(&value, sizeof(value));
		return value;
	}

	uint16_t BinaryReader::ReadUInt16()
	{
		uint16_t value;
		Read(&value, sizeof(value));
		return value;
	}

	UInt32 BinaryReader::ReadUInt32()
	{
		UInt32 value;
		Read(&value, sizeof(value));
		return value;
	}

	int8_t BinaryReader::ReadInt8()
	{
		int8_t value;
		Read(&value, sizeof(value));
		return value;
	}

	int16_t BinaryReader::ReadInt16()
	{
		int16_t value;
		Read(&value, sizeof(value));
		return value;
	}

	int32_t BinaryReader::ReadInt32()
	{
		int32_t value;
		Read(&value, sizeof(value));
		return value;
	}

	Vector2 BinaryReader::ReadVector2()
	{
		Vector2 value;
		Read(&value, sizeof(value));
		return value;
	}

	Vector3 BinaryReader::ReadVector3()
	{
		Vector3 value;
		Read(&value, sizeof(value));
		return value;
	}

	Vector4 BinaryReader::ReadVector4()
	{
		Vector4 value;
		Read(&value, sizeof(value));
		return value;
	}

	size_t BinaryReader::Read(void* destination, USize size)
	{
		if (m_cursor + size > m_raw_data.size())
			return 0;
		std::memcpy(destination, &m_raw_data[m_cursor], size);
		m_cursor += size;
		return size;
	}

	size_t BinaryReader::GetFileSize() const
	{
		return m_raw_data.size();
	}

	void BinaryReader::Seek(SeekPosition position, USize offset)
	{
		switch (position)
		{
		case SeekPosition::Begin	: m_cursor = offset;	break;
		case SeekPosition::Current	: m_cursor += offset;	break;
		case SeekPosition::End		: m_cursor = m_raw_data.size(); break;
		default:
			break;
		}
	}
}