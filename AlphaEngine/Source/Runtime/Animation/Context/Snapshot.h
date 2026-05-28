#pragma once
#include "Core/Types.h"

namespace Alpha::Animation
{
    struct Snapshot
    {
        TArray<Transform> bone_transforms{}; // 骨のポーズ
        TArray<float> morph_weights{};       // モーフターゲットのウェイト


        void Blend(const Snapshot& lhs, const Snapshot& rhs, float alpha);
    };

}