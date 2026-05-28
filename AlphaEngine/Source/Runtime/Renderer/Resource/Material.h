#pragma once
#include "Core/Types.h"
#include "Renderer/Definitions.h"

namespace Alpha
{
	class ConstantBuffer;
	struct DescriptorHandle;
	class DescriptorTable;
}

namespace Alpha::Renderer
{
	class Texture2D;

	struct TextureBinding
	{
		ETextureUsage usage{};
		TShared<Texture2D> asset{}; // 
	};

	class Material
	{
	private:

		MaterialData m_data{};
		TArray<TextureBinding> m_bindings{};

		TUnique<DescriptorTable> m_texture_table{};
		TUnique<ConstantBuffer> m_constant_buffer{};

	public:
		~Material();
		// 基本色（Albedo/Diffuse）設定
		void SetBaseColor(const Color3& color) { m_data.base_color = color; }

		// 不透明度設定
		void SetOpacity(float opacity) { m_data.opacity = opacity; }

		// 金属感設定
		void SetMetallic(float metallic) { m_data.metallic = metallic; }

		// 粗さ設定 (0.0: ツルツル, 1.0: ザラザラ)
		void SetRoughness(float roughness) { m_data.roughness = roughness; }

		// 自発光強度設定
		void SetEmissiveIntensity(float intensity) { m_data.emissive_intensity = intensity; }

		// 自発光色設定
		void SetEmissiveColor(const Color3& color) { m_data.emissive_color = color; }

		// 屈折率設定
		void SetRefractionIndex(float index) { m_data.refraction_index = index; }

		// アンビエントオクルージョン（遮蔽）設定
		void SetOcclusion(float occlusion) { m_data.occlusion = occlusion; }

		void SetData(const MaterialData& data) { m_data = data; }

		void SetConstantBuffer(TUnique<ConstantBuffer> buffer);

		void AddTextureBinding(TextureBinding texture) { m_bindings.push_back(texture); }
		
		void SetDescriptorTable(TUnique<DescriptorTable> table);

		const MaterialData& GetData() const { return m_data; }

		ConstantBuffer* GetBuffer() const { return m_constant_buffer.get(); }

		const DescriptorHandle& GetTextureHandle() const;

		const TArray<TextureBinding>& GetTextureBindings() const { return m_bindings; }
	};

}