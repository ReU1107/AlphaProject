#pragma once
#include "Vector.h"

namespace Alpha
{
	/*
	x,y,z,wの順番で格納
	*/
	// pure imaginary // 純虚四元数
	// pure reality // 純実四元数

	// 四元数
	class Quaternion
	{
	private:
		union
		{
			struct
			{
				Vector3 imaginary;	// 虚数(ベクトル表現)
				Float real;		// 実数
			};
			struct
			{
				Float x, y, z, w;
			};
			Vector4 v;
			Float t[4]{};
		};
	public:
		static const Quaternion Identity;

		Quaternion()
			: imaginary(0.0f, 0.0f, 0.0f)
			, real(1.0f)
		{
		}

		Quaternion(Float x_, Float y_, Float z_, Float w_)
			: x(x_)
			, y(y_)
			, z(z_)
			, w(w_)
		{

		}

		Quaternion(const Vector3& imaginary, Float real)
			: imaginary(imaginary)
			, real(real)
		{
		}

		inline Quaternion& operator=(const Quaternion& other)
		{
			this->real = other.real;
			this->imaginary = other.imaginary;
			return *this;
		}

		inline Quaternion& operator+=(const Quaternion& other);

		inline Quaternion& operator*=(const Quaternion& other);

		// 実数倍
		inline Quaternion& operator*=(const Float real);

		// 実数での除算
		inline Quaternion& operator/=(const Float real);

		inline Float& operator[](const UInt32 index);
		inline Float operator[](const UInt32 index) const;

		inline Float At(const UInt32 index) const;


		inline Float& X() { return x; }
		inline Float& Y() { return y; }
		inline Float& Z() { return z; }
		inline Float& W() { return w; }

		inline Float X() const { return x; }
		inline Float Y() const { return y; }
		inline Float Z() const { return z; }
		inline Float W() const { return w; }

	};

	inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);

	inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);

	inline Quaternion operator*(const Quaternion& lhs, const Float rhs);



	inline Quaternion& Quaternion::operator+=(const Quaternion& other)
	{
		this->real += other.real;
		this->imaginary += other.imaginary;
		return *this;
	}

	inline Quaternion& Quaternion::operator*=(const Quaternion& other)
	{
		const Float w1 = w;
		const Float x1 = x;
		const Float y1 = y;
		const Float z1 = z;

		const Float w2 = other.w;
		const Float x2 = other.x;
		const Float y2 = other.y;
		const Float z2 = other.z;

		this->real = (w1 * w2) - (x1 * x2) - (y1 * y2) - (z1 * z2);
		this->imaginary.X() = (w1 * x2) + (x1 * w2) + (y1 * z2) - (z1 * y2);
		this->imaginary.Y() = (w1 * y2) + (y1 * w2) + (z1 * x2) - (x1 * z2);
		this->imaginary.Z() = (w1 * z2) + (z1 * w2) + (x1 * y2) - (y1 * x2);

		return *this;
	}

	inline Quaternion& Quaternion::operator*=(const Float real)
	{
		this->real *= real;
		this->imaginary *= real;
		return *this;
	}

	inline Quaternion& Quaternion::operator/=(const Float real)
	{
		this->real /= real;
		this->imaginary /= real;
		return *this;
	}

	inline Float& Quaternion::operator[](const UInt32 index)
	{
		return t[index];
	}

	inline Float Quaternion::operator[](const UInt32 index) const
	{
		return t[index];
	}

	inline Float Quaternion::At(const UInt32 index) const
	{
		return t[index];
	}

	Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
	{
		return Quaternion(lhs) *= rhs;
	}

	Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
	{
		return Quaternion(lhs) += rhs;
	}

	Quaternion operator*(const Quaternion& lhs, const Float rhs)
	{
		return Quaternion(lhs) *= rhs;
	}
}