#pragma once
#include "AssetHandle.h"
#include "Renderer/Definitions.h"

namespace Alpha
{
	class TextureHandle;


	class MaterialHandle : public AssetHandle
	{
	private:
		Renderer::MaterialData m_data{};
		TArray<String> m_texture_paths{};

		TSharedArray<TextureHandle> m_referenced_textures{};


	public:
		~MaterialHandle();

		static AssetType Type() { return AssetType::Material; }

		AssetType GetType() const final { return AssetType::Material; }

		Renderer::MaterialData& GetData() { return m_data; }
		const Renderer::MaterialData& GetData() const { return m_data; }

		void AddTexturePath(StringView path) { m_texture_paths.push_back(path); }
		const TArray<String>& GetTexturePaths() const { return m_texture_paths; }

		void LinkTexture(TShared<TextureHandle> texture) { m_referenced_textures.push_back(texture); }

		TSharedArray<TextureHandle>& GetTextureHandles() { return m_referenced_textures; }

		const TSharedArray<TextureHandle>& GetTextureHandles() const { return m_referenced_textures; }

		void Accept(IVisitor* visitor) override {}

	};

}