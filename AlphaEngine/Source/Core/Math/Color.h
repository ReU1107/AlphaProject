#pragma once
#include "Vector.h"

namespace Alpha
{
	class Color3 : public TVector<Float, 3>
	{
	private:
		using Base = TVector<Float, 3>;
	public:
		static const Color3 Black;
		static const Color3 Gray;
		static const Color3 White;
		static const Color3 Silver;
		static const Color3 Gold;
		static const Color3 Red;
		static const Color3 Green;
		static const Color3 Blue;
		static const Color3 Yellow;
		static const Color3 Cyan;
		static const Color3 Magenta;
		static const Color3 Russet;
		static const Color3 Peru;
		static const Color3 Fawn;
		static const Color3 MellowApricot;
		static const Color3 NavajoWhite;

		Color3() = default;

		Color3(Float r, Float g, Float b)
			: Base(r, g, b)
		{
		}

		Color3(UInt8 r, UInt8 g, UInt8 b)
			: Base(r, g, b)
		{
		}

		Color3(UInt32 rgb)
			: Base()
		{
			value[0] = static_cast<Float>(rgb >> 24) / 255.0f;
			value[1] = static_cast<Float>((rgb & 0x00ff0000) >> 16) / 255.0f;
			value[2] = static_cast<Float>((rgb & 0x0000ff00) >> 8) / 255.0f;
		}
	};

	class Color4 : public TVector<Float, 4>
	{
	private:
		using Base = TVector<Float, 4>;
	public:
		static const Color4 Black;
		static const Color4 Gray;
		static const Color4 White;
		static const Color4 Silver;
		static const Color4 Gold;
		static const Color4 Red;
		static const Color4 Green;
		static const Color4 Blue;
		static const Color4 Yellow;
		static const Color4 Cyan;
		static const Color4 Magenta;
		static const Color4 Russet;
		static const Color4 Peru;
		static const Color4 Fawn;
		static const Color4 MellowApricot;
		static const Color4 NavajoWhite;

		Color4() = default;

		Color4(Float r, Float g, Float b, Float a)
			: Base(r, g, b, a)
		{
		}

		Color4(UInt32 rgba)
			: Base()
		{
			value[0] = static_cast<Float>( rgba >> 24)					/ 255.0f; // R
			value[1] = static_cast<Float>((rgba & 0x00ff0000) >> 16)	/ 255.0f; // G
			value[2] = static_cast<Float>((rgba & 0x0000ff00) >> 8)		/ 255.0f; // B
			value[3] = static_cast<Float>( rgba & 0x000000ff)			/ 255.0f; // A
		}

	};

	using color3 = Color3;
	using color4 = Color4;

}