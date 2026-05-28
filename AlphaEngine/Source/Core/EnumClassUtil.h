#pragma once
#include <type_traits>
#include <array>

namespace Alpha
{
	// 列挙を基本型にキャスト
	template<class T>
	inline std::underlying_type_t<T> CovertEnumClassToBaseType(T e)
	{
		return static_cast<std::underlying_type_t<T>>(e);
	}

#define BitFlags(Shift) (1 << Shift)


#define EnumClassFlags(Flag) \
inline Flag operator |(Flag lhs, Flag rhs)	{ return static_cast<Flag>(CovertEnumClassToBaseType(lhs) | CovertEnumClassToBaseType(rhs)); }\
inline Flag operator ^(Flag lhs, Flag rhs)	{ return static_cast<Flag>(CovertEnumClassToBaseType(lhs) ^ CovertEnumClassToBaseType(rhs)); }\
inline bool operator &(Flag lhs, Flag rhs)	{ return CovertEnumClassToBaseType(lhs) & CovertEnumClassToBaseType(rhs); }\
inline Flag operator |=(Flag& lhs, Flag rhs){ lhs = lhs | rhs;return lhs; }\
inline Flag operator ^=(Flag& lhs, Flag rhs){ lhs = lhs ^ rhs;return lhs; }

	// ビットにフラグが立っているか(EnumClassFlags必須)
	template<typename EnumClass>
	inline bool EnumHasFlag(EnumClass bit, EnumClass flag)
	{
		return ((bit & flag) != 0);
	}

	// ビットにフラグを立てる(EnumClassFlags必須)
	template<typename EnumClass>
	inline EnumClass OnBit(EnumClass& bit, EnumClass flag)
	{
		return (bit |= flag);
	}

	template<typename EnumClass>
	inline EnumClass OffBit(EnumClass& bit, EnumClass flag)
	{
		return (bit ^= flag);
	}

	// 列挙のindexに変換するために+演算子をハック
#define EnumAsIndex(Enum) \
    inline std::underlying_type_t<Enum> operator+(Enum e) { return CovertEnumClassToBaseType(e); }

#define EnumAsArray(Enum) \
	template<typename T, size_t N> \
	inline T& operator[](std::array<T, N>& arr, Enum e) { return arr[CovertEnumClassToBaseType(e)]; } \
	template<typename T, size_t N> \
	inline const T& operator[](const std::array<T, N>& arr, Enum e) { return arr[CovertEnumClassToBaseType(e)]; }

	// タイプをビットフラグに変換
#define EnumFlagsFromType(Flags, Type)\
inline Flags Type##ToEnumFlags(Type type) { return static_cast<Flags>(1 << CovertEnumClassToBaseType(type)); }
}