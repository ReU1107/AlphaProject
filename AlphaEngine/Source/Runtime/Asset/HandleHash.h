#pragma once
#include "Core/Types.h"

namespace Alpha
{
	class TextureHandle;
	class MaterialHandle;

	struct TextureHandleHash
	{
		using is_transparent = void;
		// ハッシュ用
		size_t operator()(const TextureHandle* texture) const;
		size_t operator()(const TShared<TextureHandle>& texture) const;

		// 比較演算用
		bool operator()(const TextureHandle* lhs, const TextureHandle* rhs) const;
		bool operator()(const TShared<TextureHandle>& lhs, const TShared<TextureHandle>& rhs) const;
		bool operator()(const TShared<TextureHandle>& lhs, const TextureHandle* rhs) const;
		bool operator()(const TextureHandle* lhs, const TShared<TextureHandle>& rhs) const;
	};

	struct MaterialHandleHash
	{
		using is_transparent = void;
		// ハッシュ用
		size_t operator()(const MaterialHandle* material) const;
		size_t operator()(const TShared<MaterialHandle>& material) const;

		// 比較演算用
		bool operator()(const MaterialHandle* lhs, const MaterialHandle* rhs) const;
		bool operator()(const TShared<MaterialHandle>& lhs, const TShared<MaterialHandle>& rhs) const;
		bool operator()(const TShared<MaterialHandle>& lhs, const MaterialHandle* rhs) const;
		bool operator()(const MaterialHandle* lhs, const TShared<MaterialHandle>& rhs) const;

	};
}