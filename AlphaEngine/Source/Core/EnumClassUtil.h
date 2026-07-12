#pragma once
#include <type_traits>
#include <array>

namespace Alpha
{
	// 列挙を基本型にキャスト
	template<class T>
	constexpr inline std::underlying_type_t<T> CovertEnumClassToBaseType(T e)
	{
		return static_cast<std::underlying_type_t<T>>(e);
	}

#define BitFlags(Shift) (1 << Shift)


#define EnumClassFlags(Flag) \
constexpr inline Flag operator |(Flag lhs, Flag rhs)	{ return static_cast<Flag>(CovertEnumClassToBaseType(lhs) | CovertEnumClassToBaseType(rhs)); }\
constexpr inline Flag operator ^(Flag lhs, Flag rhs)	{ return static_cast<Flag>(CovertEnumClassToBaseType(lhs) ^ CovertEnumClassToBaseType(rhs)); }\
constexpr inline bool operator &(Flag lhs, Flag rhs)	{ return CovertEnumClassToBaseType(lhs) & CovertEnumClassToBaseType(rhs); }\
constexpr inline Flag operator |=(Flag& lhs, Flag rhs){ lhs = lhs | rhs;return lhs; }\
constexpr inline Flag operator ^=(Flag& lhs, Flag rhs){ lhs = lhs ^ rhs;return lhs; }

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

	// こっちに移行させる
	template <typename Enum>
	class EnumFlags
	{
	public:
		using Underlying = std::underlying_type_t<Enum>;

		constexpr EnumFlags() noexcept : m_bits(0) {}
		constexpr EnumFlags(Enum e) noexcept : m_bits(static_cast<Underlying>(1ULL << static_cast<Underlying>(e))) {}

		constexpr EnumFlags& operator|=(EnumFlags rhs) noexcept { m_bits |= rhs.m_bits; return *this; }
		constexpr EnumFlags operator|(EnumFlags rhs) const noexcept { return EnumFlags(*this) |= rhs; }
		constexpr EnumFlags& operator&=(EnumFlags rhs) noexcept { m_bits &= rhs.m_bits; return *this; }
		constexpr EnumFlags operator&(EnumFlags rhs) const noexcept { return EnumFlags(*this) &= rhs; }

		constexpr bool Has(Enum e) const noexcept 
		{
			return (m_bits & static_cast<Underlying>(1ULL << static_cast<Underlying>(e))) != 0;
		}
		constexpr Underlying GetValue() const noexcept { return m_bits; }

	private:
		Underlying m_bits = 0;
	};

	template<typename Enum, typename = std::enable_if_t<std::is_enum_v<Enum>>>
	constexpr inline std::underlying_type_t<Enum> operator+(Enum e) 
	{
		return static_cast<std::underlying_type_t<Enum>>(e);
	}

	// enum class を添え字にできる型安全な配列
	template <typename T, typename Enum, size_t N>
	class EnumArray
	{
	public:
		constexpr T& operator[](Enum e) noexcept 
		{
			return m_data[static_cast<std::underlying_type_t<Enum>>(e)];
		}

		constexpr const T& operator[](Enum e) const noexcept 
		{
			return m_data[static_cast<std::underlying_type_t<Enum>>(e)];
		}

		constexpr std::array<T, N>& GetRaw() noexcept { return m_data; }
		constexpr const std::array<T, N>& GetRaw() const noexcept { return m_data; }

	private:
		std::array<T, N> m_data;
	};

}