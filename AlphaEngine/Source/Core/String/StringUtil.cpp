#include "StringUtil.h"
#include <filesystem>

namespace Alpha
{
	String StringUtil::ConvertU16ToString(const std::u16string& source)
	{
		return String(source);
	}

	std::string StringUtil::ConvertToUtf8(StringView str)
	{
		/*
		* std::stringがデフォルトでshit-jisになっているから
		*/
		auto u8str = std::filesystem::path(str.c_str()).u8string();
		return std::string(u8str.begin(), u8str.end());
	}

	String StringUtil::GetFolderPath(StringView file_path)
	{
		std::filesystem::path dir_path = std::filesystem::path(file_path.c_str()).parent_path() / "";
		return String(dir_path.generic_u16string());
	}
}