#pragma once
#include "AssetHandle.h"
#include "Renderer/Definitions.h"

namespace Alpha
{
	class TextureHandle;


	class MaterialHandle : public AssetHandle
	{
	public:
		struct TextureInfo
		{
			ETextureAttribute attribute{};
			String path{};
			TShared<TextureHandle> handle;
		};

	private:
		Renderer::MaterialProperties m_props{};
		TArray<TextureInfo> m_referenced_textures{};

	public:
		~MaterialHandle();

		static AssetType Type() { return AssetType::Material; }

		AssetType GetType() const final { return AssetType::Material; }

		Renderer::MaterialProperties& GetProperties() { return m_props; }
		const Renderer::MaterialProperties& GetProperties() const { return m_props; }

		void AddTexturePath(ETextureAttribute attribute, StringView path);
		void AddTexture(ETextureAttribute attribute, TShared<TextureHandle> handle);

		TArray<TextureInfo>& GetTextures() { return m_referenced_textures; }
		const TArray<TextureInfo>& GetTextures() const { return m_referenced_textures; }

		void Accept(IVisitor* visitor) override {}

	};

}