#pragma once
// Include BistroLocalLights.h at file scope first.
// Include this file where Scene/SceneObject/TransformComponent/LightComponent/
// String/Vector3/float3 resolve, just like the supplied TestLoader() implementation.

struct BistroLocalLightRig
{
    SceneObject* root = nullptr;
    std::vector<SceneObject*> objects;
    std::vector<BistroLighting::LightSpec> specs;
};

// Call once per scene import, AFTER the FBX object's parent links have been made,
// BEFORE fbx_scene is freed. Positions are already Y-up world-space meters.
// Own the returned record only while its Scene remains alive.
inline BistroLocalLightRig CreateBistroLightRig(
    Scene& scene, std::vector<BistroLighting::LightSpec> specs, const String& asset_prefix)
{
    BistroLocalLightRig rig;
    rig.specs = std::move(specs);
    if (rig.specs.empty())
        return rig;

    SceneObject& root = scene.CreateObject(asset_prefix + L"/LocalLighting");
    rig.root = &root;
    root.GetComponent<TransformComponent>().SetLocalPosition(Vector3(0, 0, 0));
    root.GetComponent<TransformComponent>().SetLocalScale(Vector3(1, 1, 1));
    rig.objects.reserve(rig.specs.size());

    for (const auto& spec : rig.specs)
    {
        // Bistro fixture names are ASCII. Their original names are preserved for inspection.
        const String suffix = std::filesystem::path(std::string(spec.name.begin(), spec.name.end())).c_str();
        SceneObject& object = scene.CreateObject(asset_prefix + L"/LocalLighting/" + suffix);
        object.SetParent(&root);

        auto& transform = object.GetComponent<TransformComponent>();
        transform.SetLocalPosition(Vector3(
            static_cast<float>(spec.position.x),
            static_cast<float>(spec.position.y),
            static_cast<float>(spec.position.z)));

        auto& light = object.AddComponent<LightComponent>();
        light.type = spec.type;
        // Same world-space direction convention as the supplied FBX light importer.
        light.direction = Vector3(
            static_cast<float>(spec.direction.x),
            static_cast<float>(spec.direction.y),
            static_cast<float>(spec.direction.z));
        light.SetColor(float3(
            static_cast<float>(spec.color.x),
            static_cast<float>(spec.color.y),
            static_cast<float>(spec.color.z)));
        light.SetIntensity(spec.intensity);
        light.SetRadius(spec.radius);

        // AlphaEngine setter computes cos(radians(input) * 0.5): pass FULL angles.
        const float angleMultiplier = 2.0f;
        light.SetSpotAngleDegree(
            spec.innerHalfAngleDeg * angleMultiplier,
            spec.outerHalfAngleDeg * angleMultiplier);
        rig.objects.push_back(&object);
    }
    // No MainLight marker and no type=2 light are created or modified here.
    return rig;
}

// Fresh scene: original 43 + all 64 string bulbs. Call once per import.
inline BistroLocalLightRig AddBistroLocalLights(Scene& scene,const ufbx_scene* fbx,
    const String& prefix,const BistroLighting::Options& options={}) {
    return CreateBistroLightRig(scene,BistroLighting::Build(fbx,options),prefix);
}
// Existing scene already has the original 43: append only string bulbs, once.
inline BistroLocalLightRig AddBistroStringLights(Scene& scene,const ufbx_scene* fbx,
    const String& prefix,const BistroLighting::Options& options={}) {
    return CreateBistroLightRig(scene,BistroLighting::BuildStringLights(fbx,options),prefix+L"/StringBulbs");
}
