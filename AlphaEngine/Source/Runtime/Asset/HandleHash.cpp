#include "HandleHash.h"
#include "MaterialHandle.h"
#include "TextureHandle.h"
#include "Core/Math/HashUtil.h"
#include "Core/Assertion.h"

namespace Alpha
{
	size_t TextureHandleHash::operator()(const TextureHandle* texture) const
	{
		if (!texture)
			return 0;
		size_t seed = 0;

		hash_combine(seed, texture->GetWidth());
		hash_combine(seed, texture->GetHeight());
		hash_combine(seed, static_cast<int>(texture->GetFormat()));

		return seed;
	}

	size_t TextureHandleHash::operator()(const TShared<TextureHandle>& texture) const
	{
		return (*this)(texture.get());
	}
	
	bool TextureHandleHash::operator()(const TextureHandle* lhs, const TextureHandle* rhs) const
	{
		return false;
	}
	
	bool TextureHandleHash::operator()(const TShared<TextureHandle>& lhs, const TShared<TextureHandle>& rhs) const
	{
		return false;
	}
	
	bool TextureHandleHash::operator()(const TShared<TextureHandle>& lhs, const TextureHandle* rhs) const
	{
		return false;
	}
	
	bool TextureHandleHash::operator()(const TextureHandle* lhs, const TShared<TextureHandle>& rhs) const
	{
		return false;
	}

	size_t Alpha::MaterialHandleHash::operator()(const MaterialHandle* material) const
	{
		if (!material)
			return 0;
		size_t seed = 0;

		const auto& data = material->GetData();
		hash_combine(seed, data.base_color.X());
		hash_combine(seed, data.base_color.Y());
		hash_combine(seed, data.base_color.Z());
		hash_combine(seed, data.opacity);
		hash_combine(seed, data.metallic);
		hash_combine(seed, data.roughness);
		hash_combine(seed, data.emissive_intensity);
		hash_combine(seed, data.occlusion);
		hash_combine(seed, data.refraction_index);

		TextureHandleHash texture_hasher;
		for (const auto& texture : material->GetTexture())
		{
			hash_combine(seed, texture_hasher(texture));
		}

		return seed;
	}
	size_t MaterialHandleHash::operator()(const TShared<MaterialHandle>& material) const
	{
		return (*this)(material.get());
	}

	bool MaterialHandleHash::operator()(const MaterialHandle* lhs, const MaterialHandle* rhs) const
	{
		return lhs->GetData() == rhs->GetData();
	}

	bool MaterialHandleHash::operator()(const TShared<MaterialHandle>& lhs, const TShared<MaterialHandle>& rhs) const
	{
		return lhs->GetData() == rhs->GetData();
	}

	bool MaterialHandleHash::operator()(const TShared<MaterialHandle>& lhs, const MaterialHandle* rhs) const
	{
		return lhs->GetData() == rhs->GetData();
	}

	bool MaterialHandleHash::operator()(const MaterialHandle* lhs, const TShared<MaterialHandle>& rhs) const
	{
		return lhs->GetData() == rhs->GetData();
	}

}