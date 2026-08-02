#pragma once
#include "Core/Common.h"
#include "Definitions.h"

namespace Alpha
{
	struct MaterialOverrideComponent
	{
		TArray<Renderer::MaterialData> materials{};

		// デバッグ用
		TArray<TString> material_names{};

	public:
		void SetBaseColor(UInt32 index, const Color3& color);
        void SetOpacity(UInt32 index, float opacity);
        void SetMetallic(UInt32 index, float metallic);
        void SetRoughness(UInt32 index, float roughness);
        void SetEmissiveColor(UInt32 index, const Color3& color);
        void SetEmissiveIntensity(UInt32 index, float intensity);
        void SetRefractionIndex(UInt32 index, float ior);

        void SetBaseColorTextureIndex(UInt32 index, UInt32 tex_idx);
        void SetNormalTextureIndex(UInt32 index, UInt32 tex_idx);
        void SetMetallicRoughnessTextureIndex(UInt32 index, UInt32 tex_idx);

        void SetMaterialName(UInt32 index, const TString& name);
        bool IsValidIndex(UInt32 index) const;
	};


}
