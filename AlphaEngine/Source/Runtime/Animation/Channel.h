#pragma once
#include "Core/Common.h"

namespace Alpha::Animation
{
	class Curve;

	class Channel
	{
	protected:
		TUniqueArray<Curve> m_curves{};

	public:
		virtual ~Channel();

		float GetEndTime() const;

		void SetCurve(uint32_t num);

		Curve* GetCurve(uint32_t index) const;

		void Sort();
	};

	class ScalarChannel final : public Channel
	{
	public:
		ScalarChannel();

		float Evaluate(float time);
	};

	class Vector3Channel final : public Channel
	{
	public:		
		Vector3Channel();

		void AddKeyframes(float time, const Vector3& value);
		void AddKeyframes(const Vector3& time, const Vector3& value);

		void AddKeyframes(float time, const Vector3& value, const Vector3& in_tangent, const Vector3& out_tangent);
		void AddKeyframes(const Vector3& time, const Vector3& value, const Vector3& in_tangent, const Vector3& out_tangent);

		Vector3 Evaluate(float time);
	};

	class QuaternionChannel final : public Channel
	{
	public:
		QuaternionChannel();

		void AddKeyframes(float time, const Vector4& value);
		void AddKeyframes(const Vector4& time, const Vector4& value);

		void AddKeyframes(float time, const Vector4& value, const Vector4& in_tangent, const Vector4& out_tangent);
		void AddKeyframes(const Vector4& time, const Vector4& value, const Vector4& in_tangent, const Vector4& out_tangent);


		Quaternion Evaluate(float time);
	};

}