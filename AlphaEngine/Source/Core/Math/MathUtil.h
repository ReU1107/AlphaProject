#pragma once
#include "Types.h"
#include "Literal.h"

#include <cmath>
#include <algorithm>
namespace Alpha
{
	// ほぼゼロか否か
	static inline bool IsNearZero(const Float value) { return (fabsf(value) <= Epsilon);  }

	// 度からラジアンに変換
	static inline Float ConvertDegreeToRadian(const Float degree) { return degree * PI / 180.0f; }

	// ラジアンから度に変換
	static inline Float ConvertRadianToDegree(const Float radian) { return radian * 180.0f / PI; }

	// 非数かどうか
	static inline bool IsNotANumber(const Float value) { return std::isnan(value); }

	// ブレンド係数
	Float BlendCoefficient(const Float current, const  Float left, const  Float right);

	// 逆線形補完(0.0～1.0f)を
	Float InverseLerp(const Float lhs, const  Float rhs, const  Float t);

	// ベジェ補間 s:始点の値(t=0の値) c1:1番目の制御点 c2:2番目の制御点 e:終点の値(t=1の値)
	Float BezierCurve(const Float s, const Float c1, const Float c2, const Float e, const Float t);

	// ベジェ補間(0～1);
	Float SimpleBezierCurve(const Float c1, const Float c2, const Float t);

	// 線形補間
	static inline Float Lerp(const Float lhs, const  Float rhs, const  Float t) { return lhs * (1.0f - t) + rhs * t;  }

	// 正規化
	static inline Float Normalize(const Float current, const Float left, const Float right) { return (current - left) / (right - left);  }

	static inline Float Abs(const Float value) { return fabsf(value); }
	static inline Float Pow(const Float b, Int32 index) { return std::powf(b, (Float)index); }
	static inline Float Cos(const Float angle) { return std::cosf(angle); }
	static inline Float Sin(const Float angle) { return std::sinf(angle); }
	static inline Float Tan(const Float angle) { return std::tanf(angle);  }

	static inline Float Ceil(const Float value) { return std::ceilf(value);  }

	static inline Float ACos(const Float t) { return std::acosf(std::clamp(t, -1.0f, 1.0f));  }
	static inline Float ASin(const Float t) { return std::asinf(std::clamp(t, -1.0f, 1.0f));  }
	static inline Float ATan(const Float t) { return std::atanf(t); }
	static inline Float ATan2(const Float y, const Float x) { return std::atan2(y, x); }

	static inline Float Sqrt(const Float t) { return std::sqrtf(t);  }

	// 第一引数が第二引数の累乗であるか
	bool IsPowerOfRadix(const UInt32 value, const UInt32 radix);

	// 引数の値が2の累乗であるか
	bool IsPowerOfTwo(const UInt32 value);

	inline float BytesToMB(UInt64 bytes)
	{
		return static_cast<float>(bytes) / (1024.0f * 1024.0f);
	}

	template<typename T, typename U>
	inline T Max(const T lhs, const T rhs, const U rt, const U rf)
	{
		return (lhs > rhs) ? rt : rf;
	}

	template<typename T>
	inline T Max(const T lhs, const T rhs)
	{
		return Max<T, T>(lhs, rhs, lhs, rhs);
	}

	template<typename T, typename U>
	inline T Min(const T lhs, const T rhs, const U rt, const U rf)
	{
		return (lhs < rhs) ? rt : rf;
	}

	template<typename T>
	inline T Min(const T lhs, const T rhs)
	{
		return Min<T, T>(lhs, rhs, lhs, rhs);
	}

	template<typename T, typename U>
	inline U Compare(const T lhs, const T rhs, const U rt, const U rf)
	{
		return (lhs == rhs) ? rt : rf;
	}

	template<typename T>
	inline T Compare(const T lhs, const T rhs)
	{
		return Compare<T, T>(lhs, rhs, lhs, rhs);
	}

	template<class T, typename U, typename V>
	inline T Clamp(const T source, const U min, const V max)
	{
		Float destination = source;
		if (source > max)
		{
			destination = max;
		}
		else if (source < min)
		{
			destination = min;
		}
		return destination;
	}

	template<typename T, typename U>
	inline T Alignment(T base, U alignment)
	{
		return ((base + alignment - 1) / alignment) * alignment;
	}

	template<typename T, typename U>
	inline T RoundUp(T value, U alignment)
	{
		return (value + alignment - 1) / alignment;
	}

	template<typename T>
	inline T SwapIndex(T& current)
	{
		return current = (1 - current);
	}

	template<typename T, typename U>
	inline void SwapIndex(T& current, const U max)
	{
		current = (current + 1) % max;
	}
}