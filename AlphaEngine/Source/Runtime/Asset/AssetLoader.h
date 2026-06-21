#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class AssetHandle;

	class AssetLoader
	{
	public:
		virtual ~AssetLoader() = default;

		virtual TShared<AssetHandle> AsyncLoad(StringView file_path) = 0;

	};


}