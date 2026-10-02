#include "VMDParser.h"
#include "Animation/Module.h"

namespace Alpha
{
	constexpr float MMD_UNIT_TO_METERS = 0.08f;
	using namespace Animation;

	VMDParser::VMDParser(StringView file_name)
	{
		m_reader.Open(file_name);

		m_reader.Read(m_header.signature, sizeof(m_header.signature));
		m_reader.Read(m_header.motionName, sizeof(m_header.motionName));

	}

	void VMDParser::Parse()
	{
		ParseBodyMotions();
		ParseMorphMotions();
	}

	void VMDParser::ParseBodyMotions()
	{
		uint32_t motion_num = 0;
		m_reader.Read(&motion_num, sizeof(motion_num));

		if (motion_num == 0)
			return;

		m_motions.resize(motion_num);

		for (auto& motion : m_motions)
		{
			m_reader.Read(motion.boneName, sizeof(motion.boneName));
			m_reader.Read(&motion.frameIndex, sizeof(motion.frameIndex));
			m_reader.Read(&motion.position, sizeof(motion.position));
			motion.position *= MMD_UNIT_TO_METERS;

			m_reader.Read(&motion.rotataion, sizeof(motion.rotataion));
			m_reader.Read(&motion.bezierControllPoint, sizeof(motion.bezierControllPoint));
		}

	}

	void VMDParser::ParseMorphMotions()
	{
		uint32_t motion_num = 0;
		m_reader.Read(&motion_num, sizeof(motion_num));

		if (motion_num == 0)
			return;

		m_morph_motions.resize(motion_num);

		for (auto& morph : m_morph_motions)
		{
			m_reader.Read(&morph.name, sizeof(morph.name));
			m_reader.Read(&morph.frameIndex, sizeof(morph.frameIndex));
			m_reader.Read(&morph.weight, sizeof(morph.weight));
		}

	}

	TUnique<Animation::SkeletalClip> VMDParser::CreateBodyMotions()
	{

		auto clip = std::make_unique<SkeletalClip>();

		std::vector<Vector2> controll_point(8);

		for (auto& body : m_motions)
		{

			// たまに\0で終わっていない文字列があるため
			body.boneName[14] = '\0';
			String target_name = Text(body.boneName);

			const float time = (float)body.frameIndex / 30.0f;
			const auto& position = body.position;
			const auto& rotation = body.rotataion;

			for (uint32_t i = 0; i < 8; ++i)
			{
				controll_point[i].X() = body.bezierControllPoint[i];
				controll_point[i].Y() = body.bezierControllPoint[i + 4];
			}

			const auto& x_in = controll_point.at(0);
			const auto& y_in = controll_point.at(1);
			const auto& z_in = controll_point.at(2);
			const auto& r_in = controll_point.at(3);

			const auto& x_out = controll_point.at(4);
			const auto& y_out = controll_point.at(5);
			const auto& z_out = controll_point.at(6);
			const auto& r_out = controll_point.at(7);

			// 接線の傾き
			Vector4 in_tangent = Vector4::Zero;
			in_tangent.X() = x_in.Y() / x_in.X();
			in_tangent.Y() = y_in.Y() / y_in.X();
			in_tangent.Z() = z_in.Y() / z_in.X();
			in_tangent.W() = r_in.Y() / r_in.X();

			Vector4 out_tangent = Vector4::Zero;
			out_tangent.X() = (float)(127 - x_out.Y()) / (127 - x_out.X());
			out_tangent.Y() = (float)(127 - y_out.Y()) / (127 - y_out.X());
			out_tangent.Z() = (float)(127 - z_out.Y()) / (127 - z_out.X());
			out_tangent.W() = (float)(127 - r_out.Y()) / (127 - r_out.X());

			TransformTrack* track = clip->FindTrack(target_name);
			Vector3Channel* trans_channel = nullptr;
			QuaternionChannel* rot_channel = nullptr;

			if (!track)
			{
				auto new_track = std::make_unique<TransformTrack>();
				new_track->SetTargetName(target_name);

				new_track->SetTranslation(std::make_unique<Vector3Channel>());
				new_track->SetRotation(std::make_unique<QuaternionChannel>());

				track = new_track.get();
				clip->AddTrack(std::move(new_track));
			}

			trans_channel = track->GetTranslation();
			trans_channel->AddKeyframes(time, position, in_tangent, out_tangent);

			rot_channel = track->GetRotation();
			rot_channel->AddKeyframes(time, rotation, in_tangent, out_tangent);

		}

		return clip;
	}

	TUnique<Animation::MorphClip> VMDParser::CreateMorphMotions()
	{
		return TUnique<Animation::MorphClip>();
	}
}