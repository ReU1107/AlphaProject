#pragma once
#include "Core/Types.h"
#include "Asset/AssetLoader.h"

namespace Alpha
{
	// VMDファイル読み込み
	class VMDLoader final : public AssetLoader
	{
	public:

		TShared<AssetHandle> AsyncLoad(StringView file_path) final;
	};

}