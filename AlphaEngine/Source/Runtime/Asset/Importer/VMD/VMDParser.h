#pragma once
#include "Core/IO/BinaryReader.h"
#include "VMDData.h"

namespace Alpha
{
	namespace Animation
	{
		class SkeletalClip;
		class MorphClip;
	}	

	class VMDParser
	{
	private:
		BinaryReader m_reader{};
		VMDHeader m_header{};
		std::vector<VMDBodyMotion> m_motions{};
		std::vector<VMDMorphMotion> m_morph_motions{};
	public:
		// コンストラクタ
		VMDParser(StringView file_name);

		void Parse();


		void ParseBodyMotions();

		void ParseMorphMotions();

		TUnique<Animation::SkeletalClip> CreateBodyMotions();

		TUnique<Animation::MorphClip> CreateMorphMotions();

	};
}