#include "Scene1.h"
#include "Runtime/Scene/Module.h"

#include "Runtime/Renderer/RenderResourceManager.h"

#include "../Components.h"
#include "Core/Math/MathF.h"

using namespace Alpha;
using namespace Alpha::Renderer;


//
//void Test()
//{
//    /*
//    entt::registry& registry = scene.GetRegistry();
//
//    auto create_mesh_object =
//        [&](const String& name,
//        UInt32 mesh_index,
//        const Vector3& position,
//        const Vector3& scale) -> SceneObject&
//    {
//        SceneObject& object =
//            scene.CreateObject(name);
//
//        auto& mesh =
//            object.AddComponent<MeshComponent>();
//
//        mesh.mesh_index = mesh_index;
//
//        auto& transform =
//            object.GetComponent<TransformComponent>();
//
//        transform.SetLocalPosition(position);
//        transform.SetLocalScale(scale);
//
//        return object;
//    };
//
//    auto add_default_materials =
//        [&](SceneObject& object,
//        UInt32 mesh_index)
//        -> MaterialOverrideComponent&
//    {
//        auto& material =
//            object.AddComponent<
//            MaterialOverrideComponent>();
//
//        material.materials =
//            RenderResourceManager::Get()
//            .GetDefaultMaterials(mesh_index);
//
//        return material;
//    };
//
//    const Vector3 default_scale(
//        1.0f,
//        1.0f,
//        1.0f);
//
//    // Akari 1
//    create_mesh_object(
//        L"Akari_1",
//        akari_index,
//        Vector3(10.0f, 0.0f, 0.0f),
//        default_scale);
//
//    // Akari 2
//    create_mesh_object(
//        L"Akari_2",
//        akari_index,
//        Vector3(10.0f, 0.0f, 10.0f),
//        default_scale);
//
//    // Patchouli
//    create_mesh_object(
//        L"Patchouli",
//        patchouli_index,
//        Vector3(-10.0f, 0.0f, 0.0f),
//        default_scale);
//
//    // Sphere
//    create_mesh_object(
//        L"Sphere",
//        sphere_index,
//        Vector3(-10.0f, 0.0f, 10.0f),
//        default_scale);
//
//    SceneObject* floor_object = nullptr;
//
//    // Floor
//    {
//        SceneObject& object =
//            create_mesh_object(
//            L"Floor",
//            cube_index,
//            Vector3(0.0f, -0.5f, 0.0f),
//            Vector3(60.0f, 1.0f, 60.0f));
//
//        add_default_materials(
//            object,
//            cube_index);
//
//        floor_object = &object;
//    }
//
//    // Ceiling
//    {
//        SceneObject& object =
//            create_mesh_object(
//            L"Ceiling",
//            cube_index,
//            Vector3(0.0f, 60.5f, 0.0f),
//            Vector3(60.0f, 1.0f, 60.0f));
//
//        add_default_materials(
//            object,
//            cube_index);
//    }
//
//    // Back wall
//    {
//        SceneObject& object =
//            create_mesh_object(
//            L"BackWall",
//            cube_index,
//            Vector3(0.0f, 30.0f, 30.5f),
//            Vector3(60.0f, 60.0f, 1.0f));
//
//        add_default_materials(
//            object,
//            cube_index);
//    }
//
//    // Left wall
//    {
//        SceneObject& object =
//            create_mesh_object(
//            L"LeftWall",
//            cube_index,
//            Vector3(-30.5f, 30.0f, 0.0f),
//            Vector3(1.0f, 60.0f, 60.0f));
//
//        auto& material =
//            add_default_materials(
//            object,
//            cube_index);
//
//        material.SetBaseColor(
//            0,
//            Color3(0.63f, 0.06f, 0.04f));
//    }
//
//    // Right wall lower
//    {
//        SceneObject& object =
//            create_mesh_object(
//            L"RightWallLower",
//            cube_index,
//            Vector3(30.5f, 10.0f, 0.0f),
//            Vector3(1.0f, 20.0f, 60.0f));
//
//        auto& material =
//            add_default_materials(
//            object,
//            cube_index);
//
//        material.SetBaseColor(
//            0,
//            Color3(0.15f, 0.48f, 0.09f));
//    }
//
//    // Right wall upper
//    {
//        SceneObject& object =
//            create_mesh_object(
//            L"RightWallUpper",
//            cube_index,
//            Vector3(30.5f, 50.0f, 0.0f),
//            Vector3(1.0f, 20.0f, 60.0f));
//
//        auto& material =
//            add_default_materials(
//            object,
//            cube_index);
//
//        material.SetBaseColor(
//            0,
//            Color3(0.15f, 0.48f, 0.09f));
//    }
//
//    // Right wall back
//    {
//        SceneObject& object =
//            create_mesh_object(
//            L"RightWallBack",
//            cube_index,
//            Vector3(30.5f, 30.0f, -20.0f),
//            Vector3(1.0f, 20.0f, 20.0f));
//
//        auto& material =
//            add_default_materials(
//            object,
//            cube_index);
//
//        material.SetBaseColor(
//            0,
//            Color3(0.15f, 0.48f, 0.09f));
//    }
//
//    // Right wall front
//    {
//        SceneObject& object =
//            create_mesh_object(
//            L"RightWallFront",
//            cube_index,
//            Vector3(30.5f, 30.0f, 20.0f),
//            Vector3(1.0f, 20.0f, 20.0f));
//
//        auto& material =
//            add_default_materials(
//            object,
//            cube_index);
//
//        material.SetBaseColor(
//            0,
//            Color3(0.15f, 0.48f, 0.09f));
//    }
//
//    SceneObject* directional_light_object = nullptr;
//
//    // Directional light
//    {
//        SceneObject& object =
//            scene.CreateObject(
//            L"DirectionalLight");
//
//        auto& light =
//            object.AddComponent<LightComponent>();
//
//        light.type = 2;
//        light.direction =
//            VectorNormalize(
//            Vector3(-1.0f, -0.5f, 0.0f));
//
//        light.SetColor(
//            float3(1.0f, 0.95f, 0.8f));
//
//        light.SetIntensity(1.0f);
//
//        directional_light_object = &object;
//    }
//
//    // Point light
//    {
//        SceneObject& object =
//            scene.CreateObject(
//            L"PointLight");
//
//        auto& transform =
//            object.GetComponent<TransformComponent>();
//
//        transform.SetLocalPosition(
//            Vector3(0.0f, 50.0f, 0.0f));
//
//        auto& light =
//            object.AddComponent<LightComponent>();
//
//        light.type = 0;
//        light.SetColor(
//            float3(1.0f, 0.0f, 0.85f));
//
//        light.SetIntensity(5.0f);
//        light.SetRadius(45.0f);
//    }
//
//    // Spot light
//    {
//        SceneObject& object =
//            scene.CreateObject(
//            L"SpotLight");
//
//        auto& transform =
//            object.GetComponent<TransformComponent>();
//
//        transform.SetLocalPosition(
//            Vector3(-10.0f, 20.0f, 10.0f));
//
//        transform.LookAt(
//            Vector3(0.0f, -1.0f, -1.0f));
//
//        auto& light =
//            object.AddComponent<LightComponent>();
//
//        light.type = 1;
//        light.SetColor(
//            float3(0.2f, 0.6f, 1.0f));
//
//        light.SetIntensity(12.0f);
//        light.SetRadius(60.0f);
//
//        light.SetSpotAngleDegree(
//            30.0f,
//            60.0f);
//    }
//
//    // Camera
//    {
//        SceneObject& object =
//            scene.CreateObject(
//            L"MainCamera");
//
//        auto& transform =
//            object.GetComponent<TransformComponent>();
//
//        transform.SetLocalPosition(
//            Vector3(0.0f, 20.0f, -20.0f));
//
//        object.AddComponent<CameraComponent>();
//    }
//
//    // Fog volume
//    {
//        SceneObject& object =
//            scene.CreateObject(
//            L"FogVolume");
//
//        auto& transform =
//            object.GetComponent<TransformComponent>();
//
//        transform.SetLocalPosition(
//            Vector3(0.0f, 25.0f, 0.0f));
//
//        auto& fog =
//            object.AddComponent<VolumeComponent>();
//
//        fog.SetSize(
//            Vector3(45.0f, 45.0f, 45.0f));
//
//        fog.SetDensity(1.5f);
//    }
//
//    {
//        SceneObject& object = scene.CreateObject(L"Emitter");
//
//        auto& transform = object.GetComponent<TransformComponent>();
//
//        transform.SetLocalPosition(Vector3(0.0f, 25.0f, 0.0f));
//
//        auto& fog = object.AddComponent<ParticleEmitterComponent>();
//        
//    }
//    */
//}

void BuildShowcaseScene(Scene& scene, UInt32 akari_index, UInt32 patchouli_index, UInt32 sphere_index, UInt32 cube_index, UInt32 bricks_index, UInt32 concrete_index, UInt32 metal_index, UInt32 rocks_index)
{

    auto create_mesh =
        [&](const String& name,
        UInt32 mesh_index,
        const Vector3& position,
        const Vector3& scale) -> SceneObject&
    {
        SceneObject& object = scene.CreateObject(name);

        auto& mesh = object.AddComponent<MeshComponent>();
        mesh.mesh_index = mesh_index;

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(position);
        transform.SetLocalScale(scale);

        return object;
    };

    auto set_material =
        [&](SceneObject& object,
        UInt32 mesh_index,
        UInt32 texture_index,
        const Color3& color,
        float roughness = 0.8f)
        -> MaterialOverrideComponent&
    {
        auto& material =
            object.AddComponent<MaterialOverrideComponent>();

        material.materials =
            RenderResourceManager::Get()
            .GetDefaultMaterials(mesh_index);

        material.SetBaseColor(0, color);
        material.SetRoughness(0, roughness);
        material.SetBaseColorTextureIndex(0, texture_index);

        return material;
    };


    // ============================================================
    // Ground
    // ============================================================

    {
        auto& floor =
            create_mesh(
            L"CourtyardFloor",
            cube_index,
            Vector3(0.0f, -0.5f, 5.0f),
            Vector3(50.0f, 1.0f, 80.0f));

        set_material(
            floor,
            cube_index,
            concrete_index,
            Color3(0.65f, 0.65f, 0.65f),
            0.95f);
    }


    // ============================================================
    // Church main building
    // ============================================================

    // Back wall
    {
        auto& wall =
            create_mesh(
            L"ChurchBackWall",
            cube_index,
            Vector3(0.0f, 10.0f, 37.0f),
            Vector3(34.0f, 20.0f, 2.0f));

        set_material(
            wall,
            cube_index,
            bricks_index,
            Color3(0.75f, 0.70f, 0.65f));
    }

    // Left church wall
    {
        auto& wall =
            create_mesh(
            L"ChurchLeftWall",
            cube_index,
            Vector3(-17.0f, 8.0f, 23.0f),
            Vector3(2.0f, 16.0f, 30.0f));

        set_material(
            wall,
            cube_index,
            bricks_index,
            Color3(0.72f, 0.68f, 0.63f));
    }

    // Right church wall
    {
        auto& wall =
            create_mesh(
            L"ChurchRightWall",
            cube_index,
            Vector3(17.0f, 8.0f, 23.0f),
            Vector3(2.0f, 16.0f, 30.0f));

        set_material(
            wall,
            cube_index,
            bricks_index,
            Color3(0.72f, 0.68f, 0.63f));
    }


    // ============================================================
    // Entrance
    //
    // 真ん中を開けて左右と上だけ作る
    // ============================================================

    {
        auto& wall =
            create_mesh(
            L"EntranceLeft",
            cube_index,
            Vector3(-11.0f, 7.0f, 8.0f),
            Vector3(12.0f, 14.0f, 2.0f));

        set_material(
            wall,
            cube_index,
            bricks_index,
            Color3(0.70f, 0.66f, 0.61f));
    }

    {
        auto& wall =
            create_mesh(
            L"EntranceRight",
            cube_index,
            Vector3(11.0f, 7.0f, 8.0f),
            Vector3(12.0f, 14.0f, 2.0f));

        set_material(
            wall,
            cube_index,
            bricks_index,
            Color3(0.70f, 0.66f, 0.61f));
    }

    {
        auto& wall =
            create_mesh(
            L"EntranceTop",
            cube_index,
            Vector3(0.0f, 14.0f, 8.0f),
            Vector3(10.0f, 4.0f, 2.0f));

        set_material(
            wall,
            cube_index,
            bricks_index,
            Color3(0.70f, 0.66f, 0.61f));
    }


    // ============================================================
    // Concrete pillars
    // ============================================================

    const float pillar_z[] =
    {
        12.0f,
        22.0f,
        32.0f
    };

    for (int i = 0; i < 3; ++i)
    {
        {
            auto& pillar =
                create_mesh(
                L"LeftPillar",
                cube_index,
                Vector3(-11.0f, 6.0f, pillar_z[i]),
                Vector3(2.5f, 12.0f, 2.5f));

            set_material(
                pillar,
                cube_index,
                concrete_index,
                Color3(0.58f, 0.58f, 0.56f));
        }

        {
            auto& pillar =
                create_mesh(
                L"RightPillar",
                cube_index,
                Vector3(11.0f, 6.0f, pillar_z[i]),
                Vector3(2.5f, 12.0f, 2.5f));

            set_material(
                pillar,
                cube_index,
                concrete_index,
                Color3(0.58f, 0.58f, 0.56f));
        }
    }


    // ============================================================
    // Metal beams
    // ============================================================

    {
        auto& beam =
            create_mesh(
            L"MetalBeam01",
            cube_index,
            Vector3(0.0f, 12.5f, 22.0f),
            Vector3(24.0f, 1.0f, 1.2f));

        set_material(
            beam,
            cube_index,
            metal_index,
            Color3(0.35f, 0.35f, 0.38f),
            0.3f);

        auto& material =
            beam.GetComponent<MaterialOverrideComponent>();

        material.SetMetallic(0, 0.9f);
    }

    {
        auto& beam =
            create_mesh(
            L"MetalBeam02",
            cube_index,
            Vector3(0.0f, 12.5f, 32.0f),
            Vector3(24.0f, 1.0f, 1.2f));

        set_material(
            beam,
            cube_index,
            metal_index,
            Color3(0.35f, 0.35f, 0.38f),
            0.3f);

        auto& material =
            beam.GetComponent<MaterialOverrideComponent>();

        material.SetMetallic(0, 0.9f);
    }


    // ============================================================
    // Metal gate
    // ============================================================

    for (int i = 0; i < 6; ++i)
    {
        const float x =
            -5.0f + static_cast<float>(i) * 2.0f;

        auto& bar =
            create_mesh(
            L"GateBar",
            cube_index,
            Vector3(x, 5.0f, 36.0f),
            Vector3(0.35f, 10.0f, 0.5f));

        set_material(
            bar,
            cube_index,
            metal_index,
            Color3(0.25f, 0.25f, 0.27f),
            0.25f);

        auto& material =
            bar.GetComponent<MaterialOverrideComponent>();

        material.SetMetallic(0, 1.0f);
    }

    {
        auto& gate_top =
            create_mesh(
            L"GateTop",
            cube_index,
            Vector3(0.0f, 9.5f, 36.0f),
            Vector3(12.0f, 0.7f, 0.6f));

        set_material(
            gate_top,
            cube_index,
            metal_index,
            Color3(0.25f, 0.25f, 0.27f),
            0.25f);

        auto& material =
            gate_top.GetComponent<MaterialOverrideComponent>();

        material.SetMetallic(0, 1.0f);
    }


    // ============================================================
    // Broken floor / rubble
    // ============================================================

    struct Rubble
    {
        Vector3 position;
        Vector3 scale;
    };

    const Rubble rubble_list[] =
    {
        {
            Vector3(-14.0f, 0.35f,  2.0f),
            Vector3(5.0f, 0.7f,  4.0f)
        },

        {
            Vector3(-8.0f, 0.55f, 14.0f),
            Vector3(4.0f, 1.1f,  3.0f)
        },

        {
            Vector3(13.0f, 0.45f, 17.0f),
            Vector3(5.0f, 0.9f,  3.0f)
        },

        {
            Vector3(8.0f, 0.75f, 28.0f),
            Vector3(3.5f, 1.5f,  5.0f)
        },

        {
            Vector3(-13.0f, 1.0f, 34.0f),
            Vector3(6.0f, 2.0f,  2.5f)
        },

        {
            Vector3(2.0f, 0.4f, -8.0f),
            Vector3(5.0f, 0.8f,  4.0f)
        },
    };

    for (const auto& rubble : rubble_list)
    {
        auto& object =
            create_mesh(
            L"Rubble",
            cube_index,
            rubble.position,
            rubble.scale);

        set_material(
            object,
            cube_index,
            rocks_index,
            Color3(0.65f, 0.63f, 0.60f),
            1.0f);
    }


    // ============================================================
    // Broken exterior walls
    // ============================================================

    {
        auto& wall =
            create_mesh(
            L"BrokenWallLeft",
            cube_index,
            Vector3(-22.0f, 4.0f, -4.0f),
            Vector3(2.0f, 8.0f, 18.0f));

        set_material(
            wall,
            cube_index,
            bricks_index,
            Color3(0.70f, 0.66f, 0.62f));
    }

    {
        auto& wall =
            create_mesh(
            L"BrokenWallRight",
            cube_index,
            Vector3(22.0f, 3.0f, -10.0f),
            Vector3(2.0f, 6.0f, 12.0f));

        set_material(
            wall,
            cube_index,
            bricks_index,
            Color3(0.70f, 0.66f, 0.62f));
    }

    {
        auto& wall =
            create_mesh(
            L"BrokenWallRight02",
            cube_index,
            Vector3(22.0f, 5.0f, 11.0f),
            Vector3(2.0f, 10.0f, 13.0f));

        set_material(
            wall,
            cube_index,
            bricks_index,
            Color3(0.70f, 0.66f, 0.62f));
    }


    // ============================================================
    // Characters
    // ============================================================

    create_mesh(
        L"Patchouli",
        patchouli_index,
        Vector3(-6.0f, 0.0f, 1.0f),
        Vector3(1.0f, 1.0f, 1.0f));

    create_mesh(
        L"Akari01",
        akari_index,
        Vector3(5.0f, 0.0f, 15.0f),
        Vector3(1.0f, 1.0f, 1.0f));

    create_mesh(
        L"Akari02",
        akari_index,
        Vector3(-4.0f, 0.0f, 29.0f),
        Vector3(1.0f, 1.0f, 1.0f));


    // ============================================================
    // Moon / ambient directional light
    // ============================================================

    {
        SceneObject& object =
            scene.CreateObject(L"MoonLight");

        auto& light =
            object.AddComponent<LightComponent>();

        light.type = 2;

        light.direction =
            VectorNormalize(
            Vector3(-0.45f, -1.0f, 0.25f));

        light.SetColor(
            float3(
            0.28f,
            0.38f,
            0.65f));

        light.SetIntensity(5.f);
    }


    // ============================================================
    // Blue volumetric Spot
    // ============================================================

    {
        SceneObject& object =
            scene.CreateObject(L"BlueSpotLight");

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(
            Vector3(-14.0f, 14.0f, -7.0f));

        transform.LookAt(
            Vector3(
            0.45f,
            -0.20f,
            1.0f));

        auto& light =
            object.AddComponent<LightComponent>();

        light.type = 1;

        light.SetColor(
            float3(
            0.12f,
            0.32f,
            1.0f));

        light.SetIntensity(20.0f);
        light.SetRadius(75.0f);

        light.SetSpotAngleDegree(
            16.0f,
            32.0f);
    }


    // ============================================================
    // Warm church interior
    // ============================================================

    {
        SceneObject& object =
            scene.CreateObject(L"WarmInteriorLight");

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(
            Vector3(0.0f, 5.0f, 31.0f));

        auto& light =
            object.AddComponent<LightComponent>();

        light.type = 0;

        light.SetColor(
            float3(
            1.0f,
            0.25f,
            0.05f));

        light.SetIntensity(16.0f);
        light.SetRadius(27.0f);
    }


    // ============================================================
    // Purple fill
    // ============================================================

    {
        SceneObject& object =
            scene.CreateObject(L"PurpleFill");

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(
            Vector3(13.0f, 4.0f, 5.0f));

        auto& light =
            object.AddComponent<LightComponent>();

        light.type = 0;

        light.SetColor(
            float3(
            0.55f,
            0.10f,
            1.0f));

        light.SetIntensity(8.0f);
        light.SetRadius(18.0f);
    }


    // ============================================================
    // Fog volume
    // ============================================================

    {
        SceneObject& object =
            scene.CreateObject(L"MainFog");

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(
            Vector3(0.0f, 9.0f, 10.0f));

        auto& volume =
            object.AddComponent<VolumeComponent>();

        volume.SetSize(
            Vector3(
            48.0f,
            18.0f,
            75.0f));

        volume.SetDensity(0.5f);
    }


    // ============================================================
    // Camera
    //
    // 初期位置だけ。自由カメラなので適当でOK。
    // ============================================================

    {
        SceneObject& object =
            scene.CreateObject(L"MainCamera");

        auto& transform =
            object.GetComponent<TransformComponent>();

        transform.SetLocalPosition(
            Vector3(0.0f, 5.0f, -28.0f));

        transform.LookAt(
            Vector3(
            0.0f,
            -0.05f,
            1.0f));

        object.AddComponent<CameraComponent>();
    }
}
