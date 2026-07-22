#pragma once
#include "Asset/AssetLoader.h"

namespace Alpha
{

	class FBXLoader : public AssetLoader
	{

	public:
		TShared<AssetHandle> AsyncLoad(StringView file_path) final;

	};

}
