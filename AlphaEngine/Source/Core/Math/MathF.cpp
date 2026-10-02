#include "MathF.h"
#include "Literal.h"

namespace Alpha
{
	Vector3 Vector3Cross(const Vector3& lhs, const Vector3& rhs)
	{
		return Vector3(
			lhs.Y() * rhs.Z() - lhs.Z() * rhs.Y(),
			lhs.Z() * rhs.X() - lhs.X() * rhs.Z(),
			lhs.X() * rhs.Y() - lhs.Y() * rhs.X()
		);
	}

	Vector3 Vector3Orthogonal(const Vector3& vec)
	{
		static const Vector3 v = Vector3(0.99146f, 0.11664f, 0.05832f);
		return Abs(VectorDot(v, vec)) > 0.99999f ? Vector3Cross(Vector3(0.0f, 1.0f, 0.0f), vec) : Vector3Cross(v, vec);
	}

	Matrix4x4 Matrix4x4Inverse(const Matrix4x4& mat)
	{
		const float _11 = mat._11(), _12 = mat._12(), _13 = mat._13(), _14 = mat._14();
		const float _21 = mat._21(), _22 = mat._22(), _23 = mat._23(), _24 = mat._24();
		const float _31 = mat._31(), _32 = mat._32(), _33 = mat._33(), _34 = mat._34();
		const float _41 = mat._41(), _42 = mat._42(), _43 = mat._43(), _44 = mat._44();

		const float t11 = _23 * _34 * _42 - _24 * _33 * _42 + _24 * _32 * _43 - _22 * _34 * _43 - _23 * _32 * _44 + _22 * _33 * _44;
		const float t12 = _14 * _33 * _42 - _13 * _34 * _42 - _14 * _32 * _43 + _12 * _34 * _43 + _13 * _32 * _44 - _12 * _33 * _44;
		const float t13 = _13 * _24 * _42 - _14 * _23 * _42 + _14 * _22 * _43 - _12 * _24 * _43 - _13 * _22 * _44 + _12 * _23 * _44;
		const float t14 = _14 * _23 * _32 - _13 * _24 * _32 - _14 * _22 * _33 + _12 * _24 * _33 + _13 * _22 * _34 - _12 * _23 * _34;

		const float det = _11 * t11 + _21 * t12 + _31 * t13 + _41 * t14;

		if (IsNearZero(det))
			return matrix4x4::Identity;

		const float idet = 1.0f / det;

		Matrix4x4 ret = Matrix4x4::Identity;

		ret[0][0] = t11 * idet;
		ret[0][1] = t12 * idet;
		ret[0][2] = t13 * idet;
		ret[0][3] = t14 * idet;

		ret[1][0] = (_24 * _33 * _41 - _23 * _34 * _41 - _24 * _31 * _43 + _21 * _34 * _43 + _23 * _31 * _44 - _21 * _33 * _44) * idet;
		ret[1][1] = (_13 * _34 * _41 - _14 * _33 * _41 + _14 * _31 * _43 - _11 * _34 * _43 - _13 * _31 * _44 + _11 * _33 * _44) * idet;
		ret[1][2] = (_14 * _23 * _41 - _13 * _24 * _41 - _14 * _21 * _43 + _11 * _24 * _43 + _13 * _21 * _44 - _11 * _23 * _44) * idet;
		ret[1][3] = (_13 * _24 * _31 - _14 * _23 * _31 + _14 * _21 * _33 - _11 * _24 * _33 - _13 * _21 * _34 + _11 * _23 * _34) * idet;

		ret[2][0] = (_22 * _34 * _41 - _24 * _32 * _41 + _24 * _31 * _42 - _21 * _34 * _42 - _22 * _31 * _44 + _21 * _32 * _44) * idet;
		ret[2][1] = (_14 * _32 * _41 - _12 * _34 * _41 - _14 * _31 * _42 + _11 * _34 * _42 + _12 * _31 * _44 - _11 * _32 * _44) * idet;
		ret[2][2] = (_12 * _24 * _41 - _14 * _22 * _41 + _14 * _21 * _42 - _11 * _24 * _42 - _12 * _21 * _44 + _11 * _22 * _44) * idet;
		ret[2][3] = (_14 * _22 * _31 - _12 * _24 * _31 - _14 * _21 * _32 + _11 * _24 * _32 + _12 * _21 * _34 - _11 * _22 * _34) * idet;

		ret[3][0] = (_23 * _32 * _41 - _22 * _33 * _41 - _23 * _31 * _42 + _21 * _33 * _42 + _22 * _31 * _43 - _21 * _32 * _43) * idet;
		ret[3][1] = (_12 * _33 * _41 - _13 * _32 * _41 + _13 * _31 * _42 - _11 * _33 * _42 - _12 * _31 * _43 + _11 * _32 * _43) * idet;
		ret[3][2] = (_13 * _22 * _41 - _12 * _23 * _41 - _13 * _21 * _42 + _11 * _23 * _42 + _12 * _21 * _43 - _11 * _22 * _43) * idet;
		ret[3][3] = (_12 * _23 * _31 - _13 * _22 * _31 + _13 * _21 * _32 - _11 * _23 * _32 - _12 * _21 * _33 + _11 * _22 * _33) * idet;
		return ret;
	}

	Matrix4x4 Matrix4x4QuickInverse(const Matrix4x4& mat)
	{
		Matrix3x3 rot = Matrix3x3(
			Vector3(mat._1()),
			Vector3(mat._2()),
			Vector3(mat._3())
		);

		rot = MatrixTranspose(rot);

		Vector3 trans = mat._4();

		float x = -VectorDot(trans, rot._1());
		float y = -VectorDot(trans, rot._2());
		float z = -VectorDot(trans, rot._3());

		return Matrix4x4(
			Vector4(rot._1(), 0.0f),
			Vector4(rot._2(), 0.0f),
			Vector4(rot._3(), 0.0f),
			Vector4(x, y, z, 1.0f)
		);
	}

	Vector3 Vector3Transform(const Vector3& vec, const Matrix4x4& mat)
	{
		const float _11 = mat._11(), _12 = mat._12(), _13 = mat._13();
		const float _21 = mat._21(), _22 = mat._22(), _23 = mat._23();
		const float _31 = mat._31(), _32 = mat._32(), _33 = mat._33();
		const float _41 = mat._41(), _42 = mat._42(), _43 = mat._43();

		Vector3 v = Vector3::Zero;
		v.X() = vec.X() * _11 + vec.Y() * _21 + vec.Z() * _31 + 1.0f * _41;
		v.Y() = vec.X() * _12 + vec.Y() * _22 + vec.Z() * _32 + 1.0f * _42;
		v.Z() = vec.X() * _13 + vec.Y() * _23 + vec.Z() * _33 + 1.0f * _43;

		return v;
	}

	Vector3 Vector3TransformCoord(const Vector3& vec, const Matrix4x4& mat)
	{
		float x = vec.X() * mat._11() + vec.Y() * mat._21() + vec.Z() * mat._31() + mat._41();
		float y = vec.X() * mat._12() + vec.Y() * mat._22() + vec.Z() * mat._32() + mat._42();
		float z = vec.X() * mat._13() + vec.Y() * mat._23() + vec.Z() * mat._33() + mat._43();
		float w = vec.X() * mat._14() + vec.Y() * mat._24() + vec.Z() * mat._34() + mat._44();

		float invW = 1.0f / w;
		return Vector3(x * invW, y * invW, z * invW);
	}

	Vector3 Vector3TransformDir(const Vector3& vec, const Matrix4x4& mat)
	{
		const float _11 = mat._11(), _12 = mat._12(), _13 = mat._13();
		const float _21 = mat._21(), _22 = mat._22(), _23 = mat._23();
		const float _31 = mat._31(), _32 = mat._32(), _33 = mat._33();

		Vector3 result = Vector3::Zero;
		result.X() = vec.X() * _11 + vec.Y() * _21 + vec.Z() * _31;
		result.Y() = vec.X() * _12 + vec.Y() * _22 + vec.Z() * _32;
		result.Z() = vec.X() * _13 + vec.Y() * _23 + vec.Z() * _33;

		return VectorNormalize(result);
	}

	Vector3 Vector3TransformDir(const Vector3& vec, const Quaternion& q)
	{
		// q.V は (x, y, z) 部分、q.W はスカラー部分
		Vector3 q_vec(q.X(), q.Y(), q.Z());

		Vector3 t = 2.0f * Vector3Cross(q_vec, vec);
		Vector3 result = vec + (q.W() * t) + Vector3Cross(q_vec, t);

		return VectorNormalize(result);
	}

	Vector3 Vector3UnprojectNDC(const Vector3& ndc, const Matrix4x4& inv_view_proj)
	{
		Vector4 world = VectorMultiply(Vector4(ndc, 1.0f), inv_view_proj);

		Vector3 result = world.XYZ();
		result /= world.W();

		return result;
	}

	Matrix4x4 Matrix4x4RotationRollPitchYaw(const float roll, const float pitch, const float yaw)
	{
		Matrix4x4 m = Matrix4x4::Identity;

		return m;
	}

	Matrix4x4 Matrix4x4RotationX(const float angle)
	{
		Matrix4x4 m = Matrix4x4::Identity;
		m[1][1] = Cos(angle);
		m[2][2] = Cos(angle);
		m[1][2] = Sin(angle);
		m[2][1] = -Sin(angle);

		return m;
	}

	Matrix4x4 Matrix4x4RotationY(const float angle)
	{
		Matrix4x4 m = Matrix4x4::Identity;
		m[0][0] = Cos(angle);
		m[2][2] = Cos(angle);
		m[0][2] = -Sin(angle);
		m[2][0] = Sin(angle);

		return m;
	}

	Matrix4x4 Matrix4x4RotationZ(const float angle)
	{
		Matrix4x4 m = Matrix4x4::Identity;
		m[0][0] = Cos(angle);
		m[1][1] = Cos(angle);
		m[0][1] = Sin(angle);
		m[1][0] = -Sin(angle);

		return m;
	}

	Matrix4x4 Matrix4x4Scaling(const Vector3& scale)
	{
		return Matrix4x4(
			Vector4(scale.X(), 0.0f, 0.0f, 0.0f),
			Vector4(0.0f, scale.Y(), 0.0f, 0.0f),
			Vector4(0.0f, 0.0f, scale.Z(), 0.0f),
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	}

	Matrix4x4 Matrix4x4Rotation(const Quaternion& rotate)
	{
		const float x = rotate.X();
		const float y = rotate.Y();
		const float z = rotate.Z();
		const float w = rotate.W();

		const float x2 = (x * x) * 2;
		const float y2 = (y * y) * 2;
		const float z2 = (z * z) * 2;
		const float xy = (x * y) * 2;
		const float xz = (x * z) * 2;
		const float xw = (x * w) * 2;
		const float yz = (y * z) * 2;
		const float yw = (y * w) * 2;
		const float zw = (z * w) * 2;

		return Matrix4x4(
			Vector4(1.0f - y2 - z2, xy + zw, xz - yw),
			Vector4(xy - zw, 1.0f - x2 - z2, yz + xw),
			Vector4(xz + yw, yz - xw, 1.0f - x2 - y2),
			Vector4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	}

	Matrix4x4 Matrix4x4Translation(const Vector3& translation)
	{
		Matrix4x4 location = Matrix4x4::Identity;
		location._4() = Vector4(translation, 1.0f);

		return location;
	}

	Matrix4x4 Matrix4x4Transformation(const Transform& transform)
	{
		Matrix4x4 m = Matrix4x4Rotation(transform.quaternion);

		// 回転行列にスケールを適用
		m._11() *= transform.scale.X(); m._12() *= transform.scale.X(); m._13() *= transform.scale.X();
		m._21() *= transform.scale.Y(); m._22() *= transform.scale.Y(); m._23() *= transform.scale.Y();
		m._31() *= transform.scale.Z(); m._32() *= transform.scale.Z(); m._33() *= transform.scale.Z();

		// 平行移動をセット
		m._41() = transform.position.X();
		m._42() = transform.position.Y();
		m._43() = transform.position.Z();

		return m;
	}

	Matrix4x4 Matrix4x4LookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up)
	{
		// カメラから見た方向ベクトル
		const Vector3 z_axis = VectorNormalize(target - eye);				// 前 (Forward)
		const Vector3 x_axis = VectorNormalize(Vector3Cross(up, z_axis));	// 右 (Right)
		const Vector3 y_axis = Vector3Cross(z_axis, x_axis);				// 上 (Up)

		const float x = -VectorDot(eye, x_axis);
		const float y = -VectorDot(eye, y_axis);
		const float z = -VectorDot(eye, z_axis);

		return Matrix4x4(
			Vector4(x_axis.X(), y_axis.X(), z_axis.X(), 0.0f),
			Vector4(x_axis.Y(), y_axis.Y(), z_axis.Y(), 0.0f),
			Vector4(x_axis.Z(), y_axis.Z(), z_axis.Z(), 0.0f),
			Vector4(x, y, z, 1.0f)
		);
	}

	Matrix4x4 Matrix4x4PerspectiveFovLH(const float fov_degree, const float aspect, const float nearZ, const float farZ)
	{
		Matrix4x4 m = Matrix4x4::Zero;

		const float fov_radian = ConvertDegreeToRadian(fov_degree);

		// cot(fovY/2) を計算
		const float h = 1.0f / Tan(fov_radian * 0.5f);
		const float w = h / aspect;

		m._11() = w;
		m._22() = h;
		m._33() = farZ / (farZ - nearZ);
		m._34() = 1.0f; // View空間のZをWにコピー（透視除算用）
		m._43() = -nearZ * farZ / (farZ - nearZ);
		m._44() = 0.0f; // ここを0にしないと正しく透視投影されない

		return m;
	}

	Matrix4x4 Matrix4x4PerspectiveFovLHReverseZ(const float fov_degree, const float aspect, const float nearZ, const float farZ)
	{
		Matrix4x4 m = Matrix4x4::Zero;

		const float fov_radian = ConvertDegreeToRadian(fov_degree);

		const float h = 1.0f / Tan(fov_radian * 0.5f);
		const float w = h / aspect;

		m._11() = w;
		m._22() = h;
		m._33() = nearZ / (nearZ - farZ);
		m._34() = 1.0f;
		m._43() = nearZ * farZ / (farZ - nearZ);
		m._44() = 0.0f;

		return m;
	}

	Matrix4x4 Matrix4x4OrthographicLH(const float width, const float height, const float nearZ, const float farZ)
	{
		Matrix4x4 m = Matrix4x4::Zero;

		// width, height はビューボリュームの幅と高さ
		m._11() = 2.0f / width;
		m._22() = 2.0f / height;
		m._33() = 1.0f / (farZ - nearZ);
		m._43() = -nearZ / (farZ - nearZ);
		m._44() = 1.0f; // 同次座標のwを維持

		return m;
	}

	Matrix4x4 Matrix4x4OrthographicLH(const float left, const float right, const float top, const float bottom, const float nearZ, const float farZ)
	{
		return Matrix4x4(
			float4(2.0f / (right - left), 0.0f, 0.0f, 0.0f),
			float4(0.0f, 2.0f / (top - bottom), 0.0f, 0.0f),
			float4(0.0f, 0.0f, 1.0f / (farZ - nearZ), 0.0f),
			float4((right + left) / (left - right), (top + bottom) / (bottom - top), -nearZ / (farZ - nearZ), 1.0f)
		);
	}

	void MatrixDecompose(const Matrix4x4& mat, Vector3* scale, Quaternion* rotation, Vector3* position)
	{
		if (position)
		{
			(*position) = mat._4().XYZ();
		}

		Vector3 row1 = mat._1();
		Vector3 row2 = mat._2();
		Vector3 row3 = mat._3();

		float sx = VectorLength(row1);
		float sy = VectorLength(row2);
		float sz = VectorLength(row3);

		if (VectorDot(Vector3Cross(row1, row2), row3) < 0)
		{
			sx *= -1.0f;
		}

		if (scale)
		{
			(*scale) = Vector3(sx, sy, sz);
		}

		if (rotation)
		{
			if (sx != 0.0f) row1 /= sx;
			if (sy != 0.0f) row2 /= sy;
			if (sz != 0.0f) row3 /= sz;

			Matrix4x4 rotMat = Matrix4x4::Identity;
			rotMat._1().XYZ() = row1;
			rotMat._2().XYZ() = row2;
			rotMat._3().XYZ() = row3;

			(*rotation) = QuaternionRotationMatrix(rotMat);
		}
	}

	Vector3 Matrix4x4Forward(const Matrix4x4& mat)
	{
		return VectorNormalize(mat._3().XYZ());
	}

	Vector3 Matrix4x4Backward(const Matrix4x4& mat)
	{
		return -VectorNormalize(mat._3().XYZ());
	}

	Vector3 Matrix4x4Right(const Matrix4x4& mat)
	{
		return VectorNormalize(mat._1().XYZ());
	}

	Vector3 Matrix4x4Left(const Matrix4x4& mat)
	{
		return -VectorNormalize(mat._1().XYZ());
	}

	Vector3 Matrix4x4Up(const Matrix4x4& mat)
	{
		return VectorNormalize(mat._2().XYZ());
	}

	Vector3 Matrix4x4Down(const Matrix4x4& mat)
	{
		return -VectorNormalize(mat._2().XYZ());
	}

	float QuaternionLengthSq(const Quaternion& q)
	{
		const float x = q.X();
		const float y = q.Y();
		const float z = q.Z();
		const float w = q.W();

		return (x * x + y * y + z * z + w * w);
	}

	float QuaternionLength(const Quaternion& q)
	{
		return Sqrt(QuaternionLengthSq(q));
	}

	float QuaternionDot(const Quaternion& lhs, const Quaternion& rhs)
	{
		const float lx = lhs.X();
		const float ly = lhs.Y();
		const float lz = lhs.Z();
		const float lw = lhs.W();

		const float rx = rhs.X();
		const float ry = rhs.Y();
		const float rz = rhs.Z();
		const float rw = rhs.W();

		return lx * rx + ly * ry + lz * rz + lw * rw;
	}

	Quaternion QuaternionNormalize(const Quaternion& q)
	{
		const auto inv_len = 1 / QuaternionLength(q);
		return Quaternion(q.X()* inv_len, q.Y() * inv_len, q.Z() * inv_len, q.W() * inv_len);
	}

	Quaternion QuaternionRotationAxis(const Vector3& axis, const float angle)
	{
		const float s = Sin(angle / 2);
		const float c = Cos(angle / 2);

		return Quaternion(axis.X() * s, axis.Y() * s, axis.Z() * s, c);
	}

	Quaternion QuaternionRollPitchYaw(float roll, float pitch, float yaw)
	{
		// それぞれの軸の四元数作成後に合成
		Quaternion r = QuaternionRotationAxis(Vector3(0.0f, 0.0f, 1.0f), roll);
		Quaternion p = QuaternionRotationAxis(Vector3(1.0f, 0.0f, 0.0f), pitch);
		Quaternion y = QuaternionRotationAxis(Vector3(0.0f, 1.0f, 0.0f), yaw);

		return y * p * r;
	}

	Quaternion QuaternionRollPitchYaw(const Vector3& euler)
	{
		return QuaternionRollPitchYaw(euler.Z(), euler.X(), euler.Y());
	}

	Quaternion QuaternionConjugate(const Quaternion& q)
	{
		// 虚数部分を反転したもの
		return Quaternion(-q.X(), -q.Y(), -q.Z(), q.W());
	}

	Quaternion QuaternionInverse(const Quaternion& q)
	{
		const float len = QuaternionLengthSq(q);

		if (IsNearZero(len))
			return Quaternion::Identity;

		return QuaternionConjugate(q) * (1.0f / len);
	}

	Quaternion QuaternionSlerp(const Quaternion& lhs, const Quaternion& rhs, const float t)
	{
		/*
		引数の四元数が単位四元数として考える
		*/

		const float angle = QuaternionDot(lhs, rhs);
		Quaternion target = rhs;
		float dot = angle;
		if (dot < 0.0f)
		{
			dot = -dot;
			target = rhs * -1.0f;
		}

		// ほぼ同じ向きなら線形補完で事足りる
		if (dot > 0.9995f)
		{
			return QuaternionNormalize(lhs * (1.0f - t) + target * t);
		}

		float ac = ACos(dot);
		if (ac < 0.0f)
		{
			ac = -ac;
		}
		const float s = Sin(ac);
		if (s <= Epsilon)
		{
			return lhs;
		}

		const float isa = 1.0f / s;
		Quaternion q = Quaternion::Identity;

		float b = Sin((1 - t) * ac) * isa;
		float c = Sin(t * ac) * isa;

		q = (lhs * b) + (target * c);

		return q;
	}

	Quaternion QuaternionRotationMatrix(const Matrix4x4& mat)
	{
		Quaternion q = Quaternion::Identity;

		const float trace = mat._11() + mat._22() + mat._33();

		if (trace > 0.0f)
		{
			const float s = Sqrt(trace + 1.0f) * 2.0f;
			const float inv_s = 1.0f / s;

			q.W() = 0.25f * s;
			q.X() = (mat._23() - mat._32()) * inv_s;
			q.Y() = (mat._31() - mat._13()) * inv_s;
			q.Z() = (mat._12() - mat._21()) * inv_s;
		}
		else if (mat._11() > mat._22() && mat._11() > mat._33())
		{
			const float s = Sqrt(1.0f + mat._11() - mat._22() - mat._33()) * 2.0f;
			const float inv_s = 1.0f / s;

			q.W() = (mat._23() - mat._32()) * inv_s;
			q.X() = 0.25f * s;
			q.Y() = (mat._12() + mat._21()) * inv_s;
			q.Z() = (mat._13() + mat._31()) * inv_s;
		}
		else if (mat._22() > mat._33())
		{
			const float s = Sqrt(1.0f + mat._22() - mat._11() - mat._33()) * 2.0f;
			const float inv_s = 1.0f / s;

			q.W() = (mat._31() - mat._13()) * inv_s;
			q.X() = (mat._12() + mat._21()) * inv_s;
			q.Y() = 0.25f * s;
			q.Z() = (mat._23() + mat._32()) * inv_s;
		}
		else
		{
			const float s = Sqrt(1.0f + mat._33() - mat._11() - mat._22()) * 2.0f;
			const float inv_s = 1.0f / s;

			q.W() = (mat._12() - mat._21()) * inv_s;
			q.X() = (mat._13() + mat._31()) * inv_s;
			q.Y() = (mat._23() + mat._32()) * inv_s;
			q.Z() = 0.25f * s;
		}

		return QuaternionNormalize(q);
	}

	Quaternion QuaternionRotationFromTo(const Vector3& from, const Vector3& to)
	{
		Vector3 v0 = VectorNormalize(from);
		Vector3 v1 = VectorNormalize(to);

		float dot = VectorDot(v0, v1);

		if (dot < -0.9999f)
		{
			Vector3 axis = Vector3Cross(Vector3(1, 0, 0), v0);
			if (VectorLengthSq(axis) < 0.01f)
				axis = Vector3Cross(Vector3(0, 1, 0), v0);
			return QuaternionRotationAxis(VectorNormalize(axis), PI);
		}
		if (dot > 0.9999f)
		{
			return Quaternion::Identity;
		}

		Vector3 axis = Vector3Cross(v0, v1);

		float s = Sqrt((1.0f + dot) * 2.0f);
		float invS = 1.0f / s;

		return Quaternion(
			axis.X() * invS,
			axis.Y() * invS,
			axis.Z() * invS,
			s * 0.5f
		);
	}

	Quaternion QuaternionRotationLookAt(const Vector3& dir, const Vector3& up)
	{
		const Vector3 z_axis = VectorNormalize(dir);				// 前 (Forward)
		if (VectorLengthSq(z_axis) < 0.0001f)
			return Quaternion::Identity;
		const Vector3 x_axis = VectorNormalize(Vector3Cross(up, z_axis));	// 右 (Right)
		const Vector3 y_axis = Vector3Cross(z_axis, x_axis);				// 上 (Up)

		Matrix4x4 m = Matrix4x4::Identity;
		m._1().XYZ() = x_axis;
		m._2().XYZ() = y_axis;
		m._3().XYZ() = z_axis;

		return QuaternionRotationMatrix(m);
	}

	void QuaternionToAxisAngle(const Quaternion& q, Vector3* axis, float* angle)
	{
		*angle = 2.0f * ACos(q.W());

		float s = Sqrt(1.0f - q.W() * q.W());

		if (s < Epsilon)
		{
			*axis = Vector3(1.0f, 0.0f, 0.0f);
		}
		else 
		{
			// 軸を正規化して抽出
			*axis = Vector3(q.X() / s, q.Y() / s, q.Z() / s);
		}
	}

	Vector3 QuaternionToEuler(const Quaternion& q)
	{
		Vector3 angle = Vector3::Zero;

		// Pitch (X軸回転)
		float sinX = 2.0f * (q.W() * q.X() - q.Y() * q.Z()); 

		// ジンバルロック判定（±90度付近）
		if (Abs(sinX) < 0.9999f)
		{
			angle.X() = ASin(sinX);

			// Yaw (Y軸回転)
			angle.Y() = ATan2(2.0f * (q.X() * q.Z() + q.W() * q.Y()),
				1.0f - 2.0f * (q.X() * q.X() + q.Y() * q.Y()));

			// Roll (Z軸回転)
			angle.Z() = ATan2(2.0f * (q.X() * q.Y() + q.W() * q.Z()),
				1.0f - 2.0f * (q.X() * q.X() + q.Z() * q.Z()));
		}
		else
		{
			// ジンバルロック時の処理（不連続性を防ぐ）
			angle.X() = (sinX > 0) ? (PI * 0.5f) : (-PI * 0.5f);
			angle.Y() = 0.0f; // Yawを0に固定
			// 全ての回転をRollに集約
			angle.Z() = ATan2(-2.0f * (q.X() * q.Y() - q.W() * q.Z()),
				1.0f - 2.0f * (q.Y() * q.Y() + q.Z() * q.Z()));
		}

		return angle;
	}
}