#pragma once
#include "Core/Types.h"
#include "Asset/AssetLoader.h"

namespace Alpha 
{
    class DDSLoader final : public AssetLoader
    {
    public:
        TShared<AssetHandle> AsyncLoad(StringView file_path) override;
    };
}
