#pragma once
#include <memory>
#include <wrl.h>
namespace Alpha
{
    template<typename T>
    using TUnique = std::unique_ptr<T>;

    template<typename T>
    using TShared = std::shared_ptr<T>;

    template<typename T>
    using TWeak = std::weak_ptr<T>;

    using Microsoft::WRL::ComPtr;

}
