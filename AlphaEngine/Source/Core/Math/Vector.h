#pragma once
#include "Types.h"
#include "MathUtil.h"

#include <utility>

namespace Alpha
{
	template<class Type, UInt32 N> class TVector;

	template<typename Type, UInt32... Indices>
	class TVectorSwizzle
	{
	private:
		Type* data{ nullptr };	// TVectorのvalueのポインタ

	public:
		static constexpr UInt32 Size = sizeof...(Indices);
		static constexpr UInt32 IndexMap[Size > 0 ? Size : 1] = { Indices... };
		using ValueType = Type;

		TVectorSwizzle(Type* ptr)
			: data(ptr)
		{
		}

		operator TVector<Type, sizeof...(Indices)> () const
		{
			TVector<Type, Size> res;
			UInt32 i = 0;
			for (UInt32 index : IndexMap)
			{
				res[i++] = data[index];
			}
			return res;
		}

		Type operator[](UInt32 i) const
		{
			return data[IndexMap[i]];
		}

		void operator=(const TVector<Type, sizeof...(Indices)>& rhs)
		{
			UInt32 i = 0;
			for (UInt32 index : IndexMap)
			{
				data[index] = rhs[i++];
			}
		}

		void operator+=(const TVector<Type, sizeof...(Indices)>& rhs)
		{
			UInt32 i = 0;
			for (UInt32 index : IndexMap)
			{
				data[index] += rhs[i++];
			}
		}

		void operator-=(const TVector<Type, sizeof...(Indices)>& rhs)
		{
			UInt32 i = 0;
			for (UInt32 index : IndexMap)
			{
				data[index] -= rhs[i++];
			}
		}

		void operator*=(const TVector<Type, sizeof...(Indices)>& rhs)
		{
			UInt32 i = 0;
			for (UInt32 index : IndexMap)
			{
				data[index] *= rhs[i++];
			}
		}

		void operator/=(const TVector<Type, sizeof...(Indices)>& rhs)
		{
			UInt32 i = 0;
			for (UInt32 index : IndexMap)
			{
				data[index] /= rhs[i++];
			}
		}

		// スカラーとの演算
		void operator+=(const Type& scalar)
		{
			for (UInt32 index : IndexMap)
			{
				data[index] += scalar;
			}
		}

		void operator-=(const Type& scalar)
		{
			for (UInt32 index : IndexMap)
			{
				data[index] -= scalar;
			}
		}

		void operator*=(const Type& scalar)
		{
			for (UInt32 index : IndexMap)
			{
				data[index] *= scalar;
			}
		}

		void operator/=(const Type& scalar)
		{
			for (UInt32 index : IndexMap)
			{
				data[index] /= scalar;
			}
		}

		void operator=(const Type& scalar)
		{
			for (UInt32 index : IndexMap)
			{
				data[index] = scalar;
			}
		}

	};

	template<class Type, UInt32 N>
	class TVector
	{
	protected:
		Type value[N]{};

	public:
		static const TVector<Type, N> Zero;
		static const TVector<Type, N> One;
		static const TVector<Type, N> XAxis;
		static const TVector<Type, N> YAxis;
		static const TVector<Type, N> ZAxis;
		static constexpr UInt32 Size = N;
		using ValueType = Type;

		TVector() = default;

		template <typename OtherType, UInt32 OtherN>
		TVector& operator=(const TVector<OtherType, OtherN>& rhs) noexcept
		{
			constexpr UInt32 minSize = (N < OtherN) ? N : OtherN;

			for (UInt32 i = 0; i < minSize; ++i)
			{
				value[i] = static_cast<Type>(rhs.At(i));
			}

			return *this;
		}

		template<typename... Args>
		TVector(Args&&... args) noexcept
		{
			UInt32 index = 0;
			// 畳み込み演算とやらでやってみる
			(Append(index, std::forward<Args>(args)), ...);
		}

		inline Type& X() { return value[0]; }
		inline Type& Y() { return value[1]; }
		inline Type& Z() { return value[2]; }
		inline Type& W() { return value[3]; }

		inline Type X() const { return value[0]; }
		inline Type Y() const { return value[1]; }
		inline Type Z() const { return value[2]; }
		inline Type W() const { return value[3]; }

		inline auto XY() { return TVectorSwizzle<Type, 0, 1>(value); }
		inline auto XZ() { return TVectorSwizzle<Type, 0, 2>(value); }
		inline auto XW() { return TVectorSwizzle<Type, 0, 3>(value); }
		inline auto YZ() { return TVectorSwizzle<Type, 1, 2>(value); }
		inline auto YW() { return TVectorSwizzle<Type, 1, 3>(value); }
		inline auto ZW() { return TVectorSwizzle<Type, 2, 3>(value); }
		inline auto XYZ() { return TVectorSwizzle<Type, 0, 1, 2>(value); }
		

		inline TVector<Type, 2> XX() const { return { value[0], value[0] }; }
		inline TVector<Type, 2> XY() const { return { value[0], value[1] }; }
		inline TVector<Type, 2> XZ() const { return { value[0], value[2] }; }
		inline TVector<Type, 2> XW() const { return { value[0], value[3] }; }
		
		inline TVector<Type, 2> YX() const { return { value[1], value[0] }; }
		inline TVector<Type, 2> YY() const { return { value[1], value[1] }; }
		inline TVector<Type, 2> YZ() const { return { value[1], value[2] }; }
		inline TVector<Type, 2> YW() const { return { value[1], value[3] }; }
		
		inline TVector<Type, 2> ZX() const { return { value[2], value[0] }; }
		inline TVector<Type, 2> ZY() const { return { value[2], value[1] }; }
		inline TVector<Type, 2> ZZ() const { return { value[2], value[2] }; }
		inline TVector<Type, 2> ZW() const { return { value[2], value[3] }; }

		inline TVector<Type, 3> XYZ() const { return { value[0], value[1], value[2] }; }
		inline TVector<Type, 3> XZY() const { return { value[0], value[2], value[1] }; }
		inline TVector<Type, 3> YXZ() const { return { value[1], value[0], value[2] }; }
		inline TVector<Type, 3> YZX() const { return { value[1], value[2], value[0] }; }
		inline TVector<Type, 3> ZXY() const { return { value[2], value[0], value[1] }; }
		inline TVector<Type, 3> ZYX() const { return { value[2], value[1], value[0] }; }

		inline TVector<Type, 3> XXX() const { return { value[0], value[0], value[0] }; }
		inline TVector<Type, 3> YYY() const { return { value[1], value[1], value[1] }; }
		inline TVector<Type, 3> ZZZ() const { return { value[2], value[2], value[2] }; }

		inline TVector<Type, N>& operator=(const TVector<Type, N>& other);
		inline TVector<Type, N>& operator+=(const TVector<Type, N>& other);
		inline TVector<Type, N>& operator-=(const TVector<Type, N>& other);
		inline TVector<Type, N>& operator*=(const TVector<Type, N>& other);
		inline TVector<Type, N>& operator/=(const TVector<Type, N>& other);

		inline TVector<Type, N>& operator=(const Type other);
		inline TVector<Type, N>& operator+=(const Type other);
		inline TVector<Type, N>& operator-=(const Type other);
		inline TVector<Type, N>& operator*=(const Type other);
		inline TVector<Type, N>& operator/=(const Type other);

		// UnSafe
		inline Type& operator[](const UInt32 index);
		inline Type operator[](const UInt32 index) const;

		// Safe
		inline Type At(const UInt32 index) const;

		Type* data() { return value; }

		const Type* data() const { return value; }


	private:
		template<typename T>
		void Append(UInt32& index, const T& val)
		{
			if (index >= N)
				return;

			if constexpr (std::is_convertible_v<T, Type>)
			{
				value[index++] = static_cast<Type>(val);
			}
			else if constexpr (requires { T::Size; })
			{
				for (UInt32 i = 0; i < T::Size; ++i)
				{
					if (index >= N)
						break;
					value[index++] = val[i];
				}
			}
		}
	};
	template<typename T>
	concept VectorLike = requires(T v, UInt32 i)
	{
		typename T::ValueType;
		{ v[i] };
		{ T::Size } -> std::convertible_to<UInt32>;
	};

	template<VectorLike Vec>
	using VectorType = TVector<typename Vec::ValueType, Vec::Size>;

	template<typename Type, typename L, typename R, typename Func>
	requires VectorLike<L> && VectorLike<R>
	[[nodiscard]]
	constexpr auto VectorOp(const L& lhs, const R& rhs, Func func) noexcept
	{
		constexpr UInt32 N = L::Size;
		static_assert(L::Size == R::Size, "ベクトルサイズの不一致");

		TVector<Type, N> result{};
		for (UInt32 i = 0; i < N; ++i)
			result[i] = func(lhs[i], rhs[i]);
		return result;
	}

	template<typename Type, typename L, typename Func>
	requires VectorLike<L>
	[[nodiscard]]
	constexpr auto VectorOp(const L& lhs, const Type rhs, Func func) noexcept
	{
		constexpr UInt32 N = L::Size;

		TVector<Type, N> result{};
		for (UInt32 i = 0; i < N; ++i)
			result[i] = func(lhs[i], rhs);
		return result;
	}

	template<typename Type, typename R, typename Func>
	requires VectorLike<R>
	[[nodiscard]]
	constexpr auto VectorOp(const Type lhs, const R& rhs, Func func) noexcept
	{
		constexpr UInt32 N = R::Size;

		TVector<Type, N> result{};
		for (UInt32 i = 0; i < N; ++i)
			result[i] = func(lhs, rhs[i]);
		return result;
	}


#define VECTOR_ARITHMETIC_OPERATORS(OP) \
	template<VectorLike L, VectorLike R> \
	[[nodiscard]] \
	constexpr auto operator OP(const L& lhs, const R& rhs) noexcept \
	{ \
		using Type = typename L::ValueType; \
		return VectorOp<Type>(lhs, rhs, [](auto a, auto b) { return a OP b; }); \
	} \
	template<typename Type, VectorLike L> \
	requires (!VectorLike<Type>) \
	[[nodiscard]] \
	constexpr auto operator OP(const L& lhs, const Type rhs) noexcept \
	{ \
		return VectorOp<Type>(lhs, rhs, [](auto a, auto b) { return a OP b; }); \
	} \
	template<typename Type, VectorLike R> \
	requires (!VectorLike<Type>) \
	[[nodiscard]] \
	constexpr auto operator OP(const Type lhs, const R& rhs) noexcept \
	{ \
		return VectorOp<Type>(lhs, rhs, [](auto a, auto b) { return a OP b; }); \
	} 

	VECTOR_ARITHMETIC_OPERATORS(+);
	VECTOR_ARITHMETIC_OPERATORS(-);
	VECTOR_ARITHMETIC_OPERATORS(*);
	VECTOR_ARITHMETIC_OPERATORS(/);

	template<class Type, UInt32 N>
	inline bool operator<(const TVector<Type, N>& lhs, const TVector<Type, N>& rhs);
	template<class Type, UInt32 N>
	inline bool operator>(const TVector<Type, N>& lhs, const TVector<Type, N>& rhs);
	template<class Type, UInt32 N>
	inline bool operator==(const TVector<Type, N>& lhs, const TVector<Type, N>& rhs);

	template<typename Type, UInt32 N>
	inline TVector<Type, N> operator-(const TVector<Type, N>& vec)
	{
		return vec * static_cast<Type>(-1);
	}

	// エイリアス
	using Vector1 = TVector<Float, 1>;
	using Vector2 = TVector<Float, 2>;
	using Vector3 = TVector<Float, 3>;
	using Vector4 = TVector<Float, 4>;

	using float1 = Vector1;
	using float2 = Vector2;
	using float3 = Vector3;
	using float4 = Vector4;

	using UVector1 = TVector<UInt32, 1>;
	using UVector2 = TVector<UInt32, 2>;
	using UVector3 = TVector<UInt32, 3>;
	using UVector4 = TVector<UInt32, 4>;

	using uint1 = UVector1;
	using uint2 = UVector2;
	using uint3 = UVector3;
	using uint4 = UVector4;

	template<class Type, UInt32 N>
	const TVector<Type, N> TVector<Type, N>::Zero = {};
	
	// 特殊化
	template<>
	const TVector<UInt32, 1> TVector<UInt32, 1>::One = TVector<UInt32, 1>(1);
	const TVector<UInt32, 2> TVector<UInt32, 2>::One = TVector<UInt32, 2>(1, 1);
	const TVector<UInt32, 3> TVector<UInt32, 3>::One = TVector<UInt32, 3>(1, 1, 1);
	const TVector<UInt32, 4> TVector<UInt32, 4>::One = TVector<UInt32, 4>(1, 1, 1, 1);

	template<>
	const TVector<Float, 1> TVector<Float, 1>::One = TVector<Float, 1>(1.0f);
	const TVector<Float, 2> TVector<Float, 2>::One = TVector<Float, 2>(1.0f, 1.0f);
	const TVector<Float, 3> TVector<Float, 3>::One = TVector<Float, 3>(1.0f, 1.0f, 1.0f);
	const TVector<Float, 4> TVector<Float, 4>::One = TVector<Float, 4>(1.0f, 1.0f, 1.0f, 1.0f);

	template<>
	const TVector<Float, 3> TVector<Float, 3>::XAxis = TVector<Float, 3>(1.0f, 0.0f, 0.0f);
	const TVector<Float, 3> TVector<Float, 3>::YAxis = TVector<Float, 3>(0.0f, 1.0f, 0.0f);
	const TVector<Float, 3> TVector<Float, 3>::ZAxis = TVector<Float, 3>(0.0f, 0.0f, 1.0f);

#undef VECTOR_ARITHMETIC_OPERATORS

	template<class Type, UInt32 N>
	inline TVector<Type, N>& TVector<Type, N>::operator=(const TVector<Type, N>& other)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			value[i] = other.value[i];
		}

		return *this;
	}

	template<class Type, UInt32 N>
	inline TVector<Type, N>& TVector<Type, N>::operator+=(const TVector<Type, N>& other)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			value[i] += other.value[i];
		}

		return *this;
	}

	template<class Type, UInt32 N>
	inline TVector<Type, N>& TVector<Type, N>::operator-=(const TVector<Type, N>& other)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			value[i] -= other.value[i];
		}

		return *this;
	}

	template<class Type, UInt32 N>
	inline TVector<Type, N>& TVector<Type, N>::operator*=(const TVector<Type, N>& other)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			value[i] *= other.value[i];
		}
		return *this;
	}

	template<class Type, UInt32 N>
	inline TVector<Type, N>& TVector<Type, N>::operator/=(const TVector<Type, N>& other)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			value[i] /= other.value[i];
		}

		return *this;
	}

	template<class Type, UInt32 N>
	inline TVector<Type, N>& TVector<Type, N>::operator=(const Type other)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			value[i] = other;
		}

		return *this;
	}

	template<class Type, UInt32 N>
	inline TVector<Type, N>& TVector<Type, N>::operator+=(const Type other)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			value[i] += other;
		}

		return *this;
	}

	template<class Type, UInt32 N>
	inline TVector<Type, N>& TVector<Type, N>::operator-=(const Type other)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			value[i] -= other;
		}

		return *this;
	}

	template<class Type, UInt32 N>
	inline TVector<Type, N>& TVector<Type, N>::operator*=(const Type other)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			value[i] *= other;
		}

		return *this;
	}

	template<class Type, UInt32 N>
	inline TVector<Type, N>& TVector<Type, N>::operator/=(const Type other)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			value[i] /= other;
		}

		return *this;
	}

	template<class Type, UInt32 N>
	inline Type& TVector<Type, N>::operator[](const UInt32 index)
	{
		return value[index];
	}

	template<class Type, UInt32 N>
	inline Type TVector<Type, N>::operator[](const UInt32 index) const
	{
		return value[index];
	}

	template<class Type, UInt32 N>
	inline Type TVector<Type, N>::At(const UInt32 index) const
	{
		return value[index];
	}

	template<class Type, UInt32 N>
	bool operator<(const TVector<Type, N>& lhs, const TVector<Type, N>& rhs)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			if (lhs.At(i) > rhs.At(i))
				return false;
		}

		return true;
	}

	template<class Type, UInt32 N>
	bool operator>(const TVector<Type, N>& lhs, const TVector<Type, N>& rhs)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			if (lhs.At(i) < rhs.At(i))
				return false;
		}

		return true;
	}

	template<class Type, UInt32 N>
	bool operator==(const TVector<Type, N>& lhs, const TVector<Type, N>& rhs)
	{
		for (UInt32 i = 0; i < N; i++)
		{
			if (!IsNearZero(lhs[i] - rhs[i]))
				return false;
		}

		return true;
	}

}