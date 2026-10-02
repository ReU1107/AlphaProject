#pragma once
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"
#include "Transform.h"
#include "MathUtil.h"

namespace Alpha
{
	template<typename Type, UInt32 N>
	static inline TVector<Type, N> VectorMaskR(const TVector<Type, N>& vec);

	template<typename Type, UInt32 N>
	static inline TVector<Type, N> VectorMaskG(const TVector<Type, N>& vec);

	template<typename Type, UInt32 N>
	static inline TVector<Type, N> VectorMaskB(const TVector<Type, N>& vec);

	// 転置行列
	template<class Type, UInt32 R, UInt32 C>
	static inline TMatrix<Type, C, R> MatrixTranspose(const TMatrix<Type, R, C>& base);

	// Vector*Matrix
	template<class Type, UInt32 Row, UInt32 Column>
	static inline TVector<Type, Column> VectorMultiply(const TVector<Type, Row>& vec, const TMatrix<Type, Row, Column>& mat);

	template<class Type, UInt32 I, UInt32 J, UInt32 K>
	static inline TMatrix<Type, I, K> MatrixMultiply(const TMatrix<Type, I, J>& lhs, const TMatrix<Type, J, K>& rhs);

	// 外積計算
	Vector3 Vector3Cross(const Vector3& lhs, const Vector3& rhs);

	// 直交(垂直な)ベクトル
	Vector3 Vector3Orthogonal(const Vector3& vec);

	// ベクトル*行列(位置用)
	Vector3 Vector3Transform(const Vector3& vec, const Matrix4x4& mat);
	Vector3 Vector3TransformCoord(const Vector3& vec, const Matrix4x4& mat);

	// ベクトル*行列(方向用)
	Vector3 Vector3TransformDir(const Vector3& vec, const Matrix4x4& mat);

	Vector3 Vector3TransformDir(const Vector3& vec, const Quaternion& q);

	Vector3 Vector3UnprojectNDC(const Vector3& ndc, const Matrix4x4& inv_view_proj);

	// 逆行列
	Matrix4x4 Matrix4x4Inverse(const Matrix4x4& mat);

	// 逆行列(回転と並行移動のみ)
	Matrix4x4 Matrix4x4QuickInverse(const Matrix4x4& mat);

	// ロール、ピッチ、ヨー回転
	Matrix4x4 Matrix4x4RotationRollPitchYaw(const float roll, const float pitch, const float yaw);

	// X軸回転行列
	Matrix4x4 Matrix4x4RotationX(const float angle);

	// Y軸回転行列
	Matrix4x4 Matrix4x4RotationY(const float angle);

	// Z軸回転行列
	Matrix4x4 Matrix4x4RotationZ(const float angle);

	// スケーリング行列
	Matrix4x4 Matrix4x4Scaling(const Vector3& scale);

	// 回転行列
	Matrix4x4 Matrix4x4Rotation(const Quaternion& rotate);

	// 移動行列
	Matrix4x4 Matrix4x4Translation(const Vector3& translation);

	// 
	Matrix4x4 Matrix4x4Transformation(const Transform& transform);

	// 
	Matrix4x4 Matrix4x4LookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up = Vector3(0.0f, 1.0f, 0.0f));

	// 射影変換行列(左手座標系)
	Matrix4x4 Matrix4x4PerspectiveFovLH(const float fov, const float aspect, const float nearZ, const float farZ);
	Matrix4x4 Matrix4x4PerspectiveFovLHReverseZ(const float fov, const float aspect, const float nearZ, const float farZ);

	Matrix4x4 Matrix4x4OrthographicLH(const float width, const float height, const float nearZ, const float farZ);
	Matrix4x4 Matrix4x4OrthographicLH(const float left, const float right, const float top, const float bottom, const float nearZ, const float farZ);

	// 行列より要素取り出し
	void MatrixDecompose(const Matrix4x4& mat, Vector3* scale, Quaternion* rotation, Vector3* position);

	Vector3 Matrix4x4Forward(const Matrix4x4& mat);
	Vector3 Matrix4x4Backward(const Matrix4x4& mat);
	Vector3 Matrix4x4Right(const Matrix4x4& mat);
	Vector3 Matrix4x4Left(const Matrix4x4& mat);
	Vector3 Matrix4x4Up(const Matrix4x4& mat);
	Vector3 Matrix4x4Down(const Matrix4x4& mat);

	// 四元数
	float QuaternionLengthSq(const Quaternion& q);

	// 四元数
	float QuaternionLength(const Quaternion& q);

	// 
	float QuaternionDot(const Quaternion& lhs, const Quaternion& rhs);

	// 四元数正規化
	Quaternion QuaternionNormalize(const Quaternion& q);

	// 回転
	Quaternion QuaternionRotationAxis(const Vector3& axis, const float angle);

	// ロールピッチヨー
	Quaternion QuaternionRollPitchYaw(float roll, float pitch, float yaw);
	/*
	* x:Pitch
	* y:Yaw
	* z:Roll
	*/
	Quaternion QuaternionRollPitchYaw(const Vector3& euler);

	// 共役四元数
	Quaternion QuaternionConjugate(const Quaternion& q);

	// 逆四元数
	Quaternion QuaternionInverse(const Quaternion& q);

	// 回転値補間
	Quaternion QuaternionSlerp(const Quaternion& lhs, const Quaternion& rhs, const float t);

	Quaternion QuaternionRotationMatrix(const Matrix4x4& mat);

	Quaternion QuaternionRotationFromTo(const Vector3& from, const Vector3& to);

	Quaternion QuaternionRotationLookAt(const Vector3& dir, const Vector3& up);

	// 
	void QuaternionToAxisAngle(const Quaternion& quaternion, Vector3* axis, float* angle);

	// x,y,zそれぞれの回転値取得
	Vector3 QuaternionToEuler(const Quaternion& quaternion);

	template<VectorLike Vec>
	inline auto VectorLengthSq(const Vec& vec)
	{
		using Type = typename Vec::ValueType;

		Type value = {};
		for (UInt32 i = 0; i < Vec::Size; i++)
		{
			value += vec[i] * vec[i];
		}

		return value;
	}

	template<VectorLike Vec>
	inline auto VectorLength(const Vec& vec)
	{
		return Sqrt(VectorLengthSq(vec));
	}

	template<VectorLike A, VectorLike B>
	requires (A::Size == B::Size)
	inline auto VectorDot(const A& a, const B& b)
	{
		using Type = std::common_type_t<
			typename A::ValueType,
			typename B::ValueType>;

		Type result{};

		for (UInt32 i = 0; i < A::Size; ++i)
			result += a[i] * b[i];

		return result;
	}

	template<VectorLike Vec>
	inline VectorType<Vec> VectorLerp(const Vec& lhs, const Vec& rhs, const float s)
	{
		return lhs * (1.0f - s) + rhs * s;
	}

	template<VectorLike Vec>
	inline VectorType<Vec> VectorNormalize(const Vec& vec)
	{
		return vec / VectorLength(vec);
	}

	template<VectorLike Vec>
	inline VectorType<Vec> VectorParallel(const Vec& normal, const Vec& dir)
	{
		return normal * VectorDot(normal, -dir) + dir;
	}

	template<VectorLike Vec>
	inline VectorType<Vec> VectorReflect(const Vec& normal, const Vec& dir)
	{
		return normal * VectorDot(normal, -dir) * 2 + dir;
	}

	template<VectorLike Vec, class MinType, class MaxType>
	inline VectorType<Vec> VectorClamp(const Vec& vec, const MinType min, const MaxType max)
	{
		using Type = typename Vec::ValueType;

		VectorType<Vec> v;
		for (UInt32 i = 0; i < Vec::Size; i++)
		{
			v[i] = static_cast<Type>(Clamp(vec[i], min, max));
		}

		return v;
	}

	template<VectorLike Vec>
	inline VectorType<Vec> VectorClamp(const Vec& vec, const Vec& min, const Vec& max)
	{
		VectorType<Vec> v;
		for (UInt32 i = 0; i < Vec::Size; i++)
		{
			v[i] = Clamp(vec[i], min[i], max[i]);
		}

		return v;
	}

	template<VectorLike Vec>
	inline VectorType<Vec> VectorMin(const Vec& vec, const typename Vec::ValueType min)
	{
		VectorType<Vec> v;
		for (UInt32 i = 0; i < Vec::Size; i++)
		{
			v[i] = Min(vec[i], min);
		}

		return v;
	}

	template<VectorLike Vec>
	inline VectorType<Vec> VectorMin(const Vec& vec, const Vec& min)
	{
		VectorType<Vec> v;
		for (UInt32 i = 0; i < Vec::Size; i++)
		{
			v[i] = Min(vec[i], min[i]);
		}

		return v;
	}

	template<VectorLike Vec>
	inline VectorType<Vec> VectorMax(const Vec& vec, const typename Vec::ValueType max)
	{
		VectorType<Vec> v;
		for (UInt32 i = 0; i < Vec::Size; i++)
		{
			v[i] = Max(vec[i], max);
		}

		return v;
	}

	template<VectorLike Vec>
	inline VectorType<Vec> VectorMax(const Vec& vec, const Vec& max)
	{
		VectorType<Vec> v;
		for (UInt32 i = 0; i < Vec::Size; i++)
		{
			v[i] = Max(vec[i], max[i]);
		}

		return v;
	}

	template<typename Type, UInt32 N>
	inline TVector<Type, N> VectorMaskR(const TVector<Type, N>& vec)
	{
		TVector<Type, N> result(vec);
		result.Y() = static_cast<Type>(0);
		result.Z() = static_cast<Type>(0);
		return result;
	}

	template<typename Type, UInt32 N>
	inline TVector<Type, N> VectorMaskG(const TVector<Type, N>& vec)
	{
		TVector<Type, N> result(vec);
		result.X() = static_cast<Type>(0);
		result.Z() = static_cast<Type>(0);
		return result;
	}

	template<typename Type, UInt32 N>
	inline TVector<Type, N> VectorMaskB(const TVector<Type, N>& vec)
	{
		TVector<Type, N> result(vec);
		result.X() = static_cast<Type>(0);
		result.Y() = static_cast<Type>(0);
		return result;
	}

	template<class Type, UInt32 R, UInt32 C>
	TMatrix<Type, C, R> MatrixTranspose(const TMatrix<Type, R, C>& base)
	{
		// 行R 列C が 行C 列R
		TMatrix<Type, C, R> m = TMatrix<Type, C, R>::Identity;

		for (UInt32 i = 0; i < R; i++)
		{
			for (UInt32 j = 0; j < C; j++)
			{
				m[i][j] = base.At(j).At(i);
			}
		}

		return m;
	}

	template<class Type, UInt32 Row, UInt32 Column>
	TVector<Type, Column> VectorMultiply(const TVector<Type, Row>& vec, const TMatrix<Type, Row, Column>& mat)
	{
		TVector<Type, Column> v = TVector<Type, Column>::Zero;
		for (UInt32 i = 0; i < Column; i++)
		{
			for (UInt32 j = 0; j < Row; j++)
			{
				v[i] += vec.At(j) * mat.At(j).At(i);
			}
		}

		return v;
	}

	template<class Type, UInt32 I, UInt32 J, UInt32 K>
	inline TMatrix<Type, I, K> MatrixMultiply(const TMatrix<Type, I, J>& lhs, const TMatrix<Type, J, K>& rhs)
	{
		TMatrix<Type, I, K> result = TMatrix<Type, I, K>::Zero;

		for (UInt32 i = 0; i < I; i++)
		{
			for (UInt32 j = 0; j < J; j++)
			{
				Type leftValue = lhs.At(i).At(j);
				for (UInt32 k = 0; k < K; k++)
				{
					result[i][k] += leftValue * rhs.At(j).At(k);
				}
			}
		}

		return result;
	}

}