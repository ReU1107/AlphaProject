#include "MaterialHandle.h"

namespace Alpha
{
	MaterialHandle::~MaterialHandle() = default;

	void MaterialHandle::AddTexturePath(ETextureAttribute attribute, StringView path)
	{
		TextureInfo info = {};
		info.attribute = attribute;
		info.path = path;
		m_referenced_textures.push_back(info);
	}

	void MaterialHandle::AddTexture(ETextureAttribute attribute, TShared<TextureHandle> handle, ESamplerPresetType sampler)
	{
		TextureInfo info = {};
		info.attribute = attribute;
		info.handle = handle;
		info.sampler = sampler;
		m_referenced_textures.push_back(info);
	}
}