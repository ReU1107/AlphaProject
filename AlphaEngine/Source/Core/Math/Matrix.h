#pragma once
#include "Vector.h"


namespace Alpha
{
	// 行アクセス用マクロ
#define MATRIX_ROW_ACCESSOR(r) \
    inline TVector<Type, Column>& _##r() requires (Row >= r) { return vec[r-1]; } \
    inline TVector<Type, Column>  _##r() const requires (Row >= r) { return vec[r-1]; }

	// 行列アクセス用マクロ
#define MATRIX_ACCESSOR(r, c) \
    inline Type& _##r##c() requires (Row >= r && Column >= c) { \
        return vec[r-1][c-1]; \
    } \
    inline Type _##r##c() const requires (Row >= r && Column >= c) { \
        return vec[r-1][c-1]; \
    }

	/*
	Row:行
	Column:列
	*/

	template<class Type, UInt32 Row, UInt32 Column>
	class TMatrix
	{
	private:
		union
		{
			TVector<Type, Column> vec[Row];
			Type value[Row * Column]{};
		};
	public:
		static const TMatrix<Type, Row, Column> Zero;
		static const TMatrix<Type, Row, Column> Identity;

		static constexpr UInt32 N = Row * Column;
		static constexpr UInt32 RowSize = Row;
		static constexpr UInt32 ColumnSize = Column;


		TMatrix() {}

		template<typename... Args>
		TMatrix(Args... args)
		{
			UInt32 index = 0;
			// 畳み込み演算とやらでやってみる
			(Append(index, std::forward<Args>(args)), ...);
		}

		// _〇()
		MATRIX_ROW_ACCESSOR(1);
		MATRIX_ROW_ACCESSOR(2);
		MATRIX_ROW_ACCESSOR(3);
		MATRIX_ROW_ACCESSOR(4);

		// _1〇()
		MATRIX_ACCESSOR(1, 1);
		MATRIX_ACCESSOR(1, 2);
		MATRIX_ACCESSOR(1, 3);
		MATRIX_ACCESSOR(1, 4);
		// _2〇()
		MATRIX_ACCESSOR(2, 1);
		MATRIX_ACCESSOR(2, 2);
		MATRIX_ACCESSOR(2, 3);
		MATRIX_ACCESSOR(2, 4);
		// _3〇()
		MATRIX_ACCESSOR(3, 1);
		MATRIX_ACCESSOR(3, 2);
		MATRIX_ACCESSOR(3, 3);
		MATRIX_ACCESSOR(3, 4);
		// _4〇()
		MATRIX_ACCESSOR(4, 1);
		MATRIX_ACCESSOR(4, 2);
		MATRIX_ACCESSOR(4, 3);
		MATRIX_ACCESSOR(4, 4);


		inline TMatrix<Type, Row, Column>& operator=(const TMatrix<Type, Row, Column>& other);
		inline TMatrix<Type, Row, Column>& operator*=(const TMatrix<Type, Row, Column>& other);

		inline TVector<Type, Column>& operator[](const UInt32 row_index);
		inline TVector<Type, Column> operator[](const UInt32 row_index) const;

		inline TVector<Type, Column> At(const UInt32 row_index) const;
		
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
			else if constexpr (requires{ T::RowSize; T::ColumnSize; })
			{
				for (UInt32 row = 0; row < T::RowSize; ++row)
				{
					for (UInt32 column = 0; column < T::ColumnSize; ++column)
					{
						if (index >= N) 
							break;
						value[index++] = val[row][column];
					}
				}
			}
		}
	};

	template<class Type, UInt32 Row, UInt32 Column>
	static inline TMatrix<Type, Row, Column> operator*(const TMatrix<Type, Row, Column>& lhs, const TMatrix<Type, Row, Column>& rhs);

	using Matrix2x2 = TMatrix<Float, 2, 2>;
	using Matrix3x3 = TMatrix<Float, 3, 3>;
	using Matrix3x4 = TMatrix<Float, 3, 4>;
	using Matrix4x3 = TMatrix<Float, 4, 3>;
	using Matrix4x4 = TMatrix<Float, 4, 4>;

	using matrix2x2 = Matrix2x2;
	using matrix3x3 = Matrix3x3;
	using matrix3x4 = Matrix3x4;
	using matrix4x3 = Matrix4x3;
	using matrix4x4 = Matrix4x4;

	using Float2x2 = Matrix2x2;
	using Float3x3 = Matrix3x3;
	using Float3x4 = Matrix3x4;
	using Float4x3 = Matrix4x3;
	using Float4x4 = Matrix4x4;

	using float2x2 = Matrix2x2;
	using float3x3 = Matrix3x3;
	using float3x4 = Matrix3x4;
	using float4x3 = Matrix4x3;
	using float4x4 = Matrix4x4;

	// 特殊化
	template<class Type, UInt32 Row, UInt32 Column>
	const TMatrix<Type, Row, Column> TMatrix<Type, Row, Column>::Zero = {};

	template<>
	const TMatrix<Float, 2, 2> TMatrix<Float, 2, 2>::Identity =
		TMatrix<Float, 2, 2>(
			1.0f, 0.0f,
			0.0f, 1.0f
		);

	template<>
	const TMatrix<Float, 3, 3> TMatrix<Float, 3, 3>::Identity =
		TMatrix<Float, 3, 3>(
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		);

	template<>
	const TMatrix<Float, 4, 4> TMatrix<Float, 4, 4>::Identity =
		TMatrix<Float, 4, 4>(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

#undef MATRIX_ACCESSOR
#undef MATRIX_ROW_ACCESSOR
	template<class Type, UInt32 Row, UInt32 Column>
	inline TMatrix<Type, Row, Column>& TMatrix<Type, Row, Column>::operator=(const TMatrix<Type, Row, Column>& other)
	{
		for (UInt32 i = 0; i < Row; i++)
		{
			for (UInt32 j = 0; j < Column; j++)
			{
				vec[i][j] = other.vec[i][j];
			}
		}

		return *this;
	}
	
	template<class Type, UInt32 Row, UInt32 Column>
	inline TMatrix<Type, Row, Column>& TMatrix<Type, Row, Column>::operator*=(const TMatrix<Type, Row, Column>& other)
	{
		return *this;
	}
	
	template<class Type, UInt32 Row, UInt32 Column>
	inline TVector<Type, Column>& TMatrix<Type, Row, Column>::operator[](const UInt32 row_index)
	{
		return vec[row_index];
	}
	
	template<class Type, UInt32 Row, UInt32 Column>
	inline TVector<Type, Column> TMatrix<Type, Row, Column>::operator[](const UInt32 row_index) const
	{
		return vec[row_index];
	}
	
	template<class Type, UInt32 Row, UInt32 Column>
	inline TVector<Type, Column> TMatrix<Type, Row, Column>::At(const UInt32 row_index) const
	{
		return vec[row_index];
	}

	template<class Type, UInt32 Row, UInt32 Column>
	TMatrix<Type, Row, Column> operator*(const TMatrix<Type, Row, Column>& lhs, const TMatrix<Type, Row, Column>& rhs)
	{
		return TMatrix<Type, Row, Column>();
	}
}

//#include "../../src/Math/MatrixImpl.h"