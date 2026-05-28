#include "MathUtil.h"
namespace Alpha
{

    Float BlendCoefficient(const Float current, const Float left, const Float right)
    {
        Float blend = 0.0f;

        const Float numerator = current - left;
        const Float denominator = right - left;
        // 0除算防止用
        if (IsNearZero(denominator))
            return 0.0f;

        blend = numerator / denominator;

        return blend;
    }

    Float InverseLerp(const Float lhs, const Float rhs, const Float t)
    {
        const Float denominator = rhs - lhs;
        // 0除算防止用
        if (IsNearZero(denominator))
            return 0.0f;

        return (t - lhs) / (rhs - lhs);
    }

    Float BezierCurve(const Float s, const Float c1, const Float c2, const Float e, const Float t)
    {
        const Float d = (1 - t);
        const Float d2 = d * d;
        const Float d3 = d2 * d;

        const Float t2 = t * t;
        const Float t3 = t2 * t;

        const Float b1 = d3 * s;
        const Float b2 = 3.0f * d2 * t * c1;
        const Float b3 = 3.0f * d * t2 * c2;
        const Float b4 = t3 * e;

        return b1 + b2 + b3 + b4;
    }

    Float SimpleBezierCurve(const Float c1, const Float c2, const Float t)
    {
        const Float d = (1 - t);
        const Float d2 = d * d;

        const Float t2 = t * t;
        const Float t3 = t2 * t;

        const Float b2 = 3.0f * d2 * t * c1;
        const Float b3 = 3.0f * d * t2 * c2;
        const Float b4 = t3;

        return b2 + b3 + b4;
    }

    bool IsPowerOfRadix(const UInt32 value, const UInt32 radix)
    {
        return false;
    }

    bool IsPowerOfTwo(const UInt32 value)
    {
        return IsPowerOfRadix(value, 2);
    }

}