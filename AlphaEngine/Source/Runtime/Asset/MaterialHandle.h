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

		TSharedArray<TextureHandle> m_referenced_textures{};

	public:
		~MaterialHandle();

		static AssetType Type() { return AssetType::Material; }

		AssetType GetType() const final { return AssetType::Material; }

		Renderer::MaterialData& GetData() { return m_data; }

		const Renderer::MaterialData& GetData() const { return m_data; }

		TSharedArray<TextureHandle>& GetTexture() { return m_referenced_textures; }

		const TSharedArray<TextureHandle>& GetTexture() const { return m_referenced_textures; }

		void Accept(IVisitor* visitor) override {}

	};

}