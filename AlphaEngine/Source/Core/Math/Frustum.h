#pragma once
#include "Core/Container/Array.h"
#include "Core/EnumClassUtil.h"
#include "Plane.h"

namespace Alpha
{
    enum class EFrustumSide
    {
        Left,
        Right,
        Top,
        Bottom,
        Near,
        Far,
    };

    EnumAsIndex(EFrustumSide);

    struct Frustum
    {
        TFixedArray<Plane, 6>planes{};
    };

}