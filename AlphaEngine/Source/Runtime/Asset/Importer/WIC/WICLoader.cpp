#include "WICLoader.h"
#include "Asset/TextureHandle.h"

#include "Core/String/StringUtil.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <cstdio>

namespace Alpha
{
	TShared<AssetHandle> WICLoader::AsyncLoad(StringView file_path)
	{
		FILE* fp = nullptr;
		_wfopen_s(&fp, file_path.c_str(), L"rb");

		int width, height, channels;

		unsigned char* pixels = stbi_load_from_file(fp, &width, &height, &channels, STBI_rgb_alpha);
		
		fclose(fp);
		
		if (!pixels)
		{
			return nullptr;
		}

		auto handle = std::make_shared<TextureHandle>();
		handle->SetWidth(width);
		handle->SetHeight(height);
		handle->SetFormat(ERHIResourceFormat::RGBAUnorm);
		handle->SetPixels(pixels);

		stbi_image_free(pixels);

		return handle;
	}

}
