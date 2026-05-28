#pragma once
#include "Core/Types.h"
#include "Asset/AssetLoader.h"

namespace Alpha
{
	// WICファイル読み込み
	class WICLoader final : public AssetLoader
	{
	public:

		TShared<AssetHandle> AsyncLoad(StringView file_path) final;
	};

}