#pragma once
#include "String.h"
namespace Alpha
{
	class StringUtil
	{
	public:
		static String ConvertU16ToString(const std::u16string& source);

		static std::string ConvertToUtf8(StringView str);

		static String GetFolderPath(StringView file_path);

	};

}