#include "MaterialOverrideComponent.h"
namespace Alpha
{
    bool MaterialOverrideComponent::IsValidIndex(UInt32 index) const
    {
        return index < static_cast<UInt32>(materials.size());
    }

	void MaterialOverrideComponent::SetBaseColor(UInt32 index, const Color3& color)
	{
        if (!IsValidIndex(index)) return;
		materials[index].props.base_color = color;
	}

    void MaterialOverrideComponent::SetOpacity(UInt32 index, float opacity)
    {
        if (!IsValidIndex(index)) return;
        materials[index].props.opacity = opacity;
    }

    void MaterialOverrideComponent::SetMetallic(UInt32 index, float metallic)
    {
        if (!IsValidIndex(index)) return;
        materials[index].props.metallic = metallic;
    }

    void MaterialOverrideComponent::SetRoughness(UInt32 index, float roughness)
    {
        if (!IsValidIndex(index)) return;
        materials[index].props.roughness = roughness;
    }

    void MaterialOverrideComponent::SetEmissiveColor(UInt32 index, const Color3& color)
    {
        if (!IsValidIndex(index)) return;
        materials[index].props.emissive_color = color;

    }

    void MaterialOverrideComponent::SetEmissiveIntensity(UInt32 index, float intensity)
    {
        if (!IsValidIndex(index)) return;
        materials[index].props.emissive_intensity = intensity;
    }

    void MaterialOverrideComponent::SetRefractionIndex(UInt32 index, float ior)
    {
        if (!IsValidIndex(index)) return;
        materials[index].props.refraction_index = ior;
    }

    void MaterialOverrideComponent::SetBaseColorTextureIndex(UInt32 index, UInt32 tex_idx)
    {
        if (!IsValidIndex(index)) return;
        materials[index].base_color_tex_idx = tex_idx;
    }

    void MaterialOverrideComponent::SetNormalTextureIndex(UInt32 index, UInt32 tex_idx)
    {
        if (!IsValidIndex(index)) return;
        materials[index].normal_tex_idx = tex_idx;
    }

    void MaterialOverrideComponent::SetMetallicRoughnessTextureIndex(UInt32 index, UInt32 tex_idx)
    {
        if (!IsValidIndex(index)) return;
        materials[index].metallic_roughness_tex_idx = tex_idx;
    }

    void MaterialOverrideComponent::SetMaterialName(UInt32 index, const TString& name)
    {
        if (index >= static_cast<UInt32>(material_names.size())) return;
        material_names[index] = name;
    }

}