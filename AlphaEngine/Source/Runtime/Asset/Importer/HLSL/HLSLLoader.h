#pragma once
#include "Asset/AssetLoader.h"

namespace Alpha
{

	class HLSLLoader : public AssetLoader
	{

	public:
		TShared<AssetHandle> AsyncLoad(StringView file_path) final;

	};

}
