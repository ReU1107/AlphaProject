#include "Channel.h"
#include "Curve.h"
#include "Core/Math/MathF.h"
#include "Core/Math/MathUtil.h"

#include <ranges>

namespace Alpha::Animation
{
	Channel::~Channel() = default;

	float Channel::GetEndTime() const
	{
		float end_time = 0.0f;

		// カーブの中で一番長いやつに合わせる
		for (auto& curve : m_curves)
		{
			end_time = Max(end_time, curve->GetEndTime());
		}

		return end_time;
	}

	void Channel::SetCurve(uint32_t num)
	{
		m_curves.resize(num);

		for (auto& curve : m_curves)
		{
			curve = std::make_unique<Curve>();
		}
	}

	Curve* Channel::GetCurve(uint32_t index) const
	{
		return m_curves[index].get();
	}

	void Channel::Sort()
	{
		for (auto& curve : m_curves)
		{
			curve->Sort();
		}
	}

	ScalarChannel::ScalarChannel()
	{
		SetCurve(1);
	}

	float ScalarChannel::Evaluate(float time)
	{
		return m_curves[0]->Evaluate(time);
	}

	Vector3Channel::Vector3Channel()
	{
		SetCurve(3);
	}

	void Vector3Channel::AddKeyframes(float time, const Vector3& value)
	{
		auto Add = [&](uint32_t index)
			{
				Keyframe keyframe = {};
				keyframe.time = time;
				keyframe.value = value[index];
				keyframe.interpolation = Interpolation::Linear;
				m_curves[index]->AddKeyframe(keyframe);
			};

		Add(0);
		Add(1);
		Add(2);
	}

	void Vector3Channel::AddKeyframes(const Vector3& time, const Vector3& value)
	{
		auto Add = [&](uint32_t index)
			{
				Keyframe keyframe = {};
				keyframe.time = time[index];
				keyframe.value = value[index];
				keyframe.interpolation = Interpolation::Linear;
				m_curves[index]->AddKeyframe(keyframe);
			};

		Add(0);
		Add(1);
		Add(2);
	}

	void Vector3Channel::AddKeyframes(float time, const Vector3& value, const Vector3& in_tangent, const Vector3& out_tangent)
	{
		auto Add = [&](uint32_t index)
			{
				Keyframe keyframe = {};
				keyframe.time = time;
				keyframe.value = value[index];
				keyframe.interpolation = Interpolation::Cubic;
				keyframe.in_tangent = in_tangent[index];
				keyframe.out_tangent = out_tangent[index];
				m_curves[index]->AddKeyframe(keyframe);
			};

		Add(0);
		Add(1);
		Add(2);
	}

	void Vector3Channel::AddKeyframes(const Vector3& time, const Vector3& value, const Vector3& in_tangent, const Vector3& out_tangent)
	{
		auto Add = [&](uint32_t index)
			{
				Keyframe keyframe = {};
				keyframe.time = time[index];
				keyframe.value = value[index];
				keyframe.interpolation = Interpolation::Cubic;
				keyframe.in_tangent = in_tangent[index];
				keyframe.out_tangent = out_tangent[index];
				m_curves[index]->AddKeyframe(keyframe);
			};

		Add(0);
		Add(1);
		Add(2);
	}

	Vector3 Vector3Channel::Evaluate(float time)
	{
		float x = m_curves[0]->Evaluate(time);
		float y = m_curves[1]->Evaluate(time);
		float z = m_curves[2]->Evaluate(time);

		return Vector3(x, y, z);
	}

	QuaternionChannel::QuaternionChannel()
	{
		SetCurve(4);
	}

	void QuaternionChannel::AddKeyframes(float time, const Vector4& value)
	{
		auto Add = [&](uint32_t index)
			{
				Keyframe keyframe = {};
				keyframe.time = time;
				keyframe.value = value[index];
				keyframe.interpolation = Interpolation::Linear;
				m_curves[index]->AddKeyframe(keyframe);
			};

		Add(0);
		Add(1);
		Add(2);
		Add(3);
	}

	void QuaternionChannel::AddKeyframes(const Vector4& time, const Vector4& value)
	{
		auto Add = [&](uint32_t index)
			{
				Keyframe keyframe = {};
				keyframe.time = time[index];
				keyframe.value = value[index];
				keyframe.interpolation = Interpolation::Linear;
				m_curves[index]->AddKeyframe(keyframe);
			};

		Add(0);
		Add(1);
		Add(2);
		Add(3);
	}

	void QuaternionChannel::AddKeyframes(float time, const Vector4& value, const Vector4& in_tangent, const Vector4& out_tangent)
	{
		auto Add = [&](uint32_t index)
			{
				Keyframe keyframe = {};
				keyframe.time = time;
				keyframe.value = value[index];
				keyframe.interpolation = Interpolation::Cubic;
				keyframe.in_tangent = in_tangent[index];
				keyframe.out_tangent = out_tangent[index];
				m_curves[index]->AddKeyframe(keyframe);
			};

		Add(0);
		Add(1);
		Add(2);
		Add(3);
	}

	void QuaternionChannel::AddKeyframes(const Vector4& time, const Vector4& value, const Vector4& in_tangent, const Vector4& out_tangent)
	{
		auto Add = [&](uint32_t index)
			{
				Keyframe keyframe = {};
				keyframe.time = time[index];
				keyframe.value = value[index];
				keyframe.interpolation = Interpolation::Cubic;
				keyframe.in_tangent = in_tangent[index];
				keyframe.out_tangent = out_tangent[index];
				m_curves[index]->AddKeyframe(keyframe);
			};

		Add(0);
		Add(1);
		Add(2);
		Add(3);
	}

	Quaternion QuaternionChannel::Evaluate(float time)
	{
		float x = m_curves[0]->Evaluate(time);
		float y = m_curves[1]->Evaluate(time);
		float z = m_curves[2]->Evaluate(time);
		float w = m_curves[3]->Evaluate(time);

		return QuaternionNormalize(Quaternion(x, y, z, w));
	}
}