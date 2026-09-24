#include "VMDLoader.h"
#include "VMDParser.h"
#include "Asset/MotionHandle.h"
#include "Animation/Module.h"

namespace Alpha
{
	using namespace Animation;

	TShared<AssetHandle> VMDLoader::AsyncLoad(StringView file_path)
	{
		VMDParser parser(file_path);

		auto handle = std::make_shared<MotionHandle>();

		parser.Parse();

		handle->m_skeletal = std::move(parser.CreateBodyMotions());
		handle->m_morph = parser.CreateMorphMotions();

		return handle;
	}

}
