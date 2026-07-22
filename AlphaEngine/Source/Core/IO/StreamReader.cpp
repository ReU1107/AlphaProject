#include "StreamReader.h"
#include <fstream>
#include <filesystem>

namespace Alpha
{
	void StreamReader::Open(StringView file_name)
	{
		std::ifstream stream(std::filesystem::path(file_name.c_str()), std::ios::ate);
	
	}
}