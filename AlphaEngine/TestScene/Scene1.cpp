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


//inline void BuildParticleShapeTestScene(
//    Scene& scene,
//    UInt32 akari_index,
//    UInt32 patchouli_index,
//    UInt32 sphere_index,
//    UInt32 cube_index)
//{
//    using Emitter = ::Alpha::Particle::ParticleEmitterComponent;
//    using Shape = ::Alpha::Particle::SourceShape;
//    using Distribution = ::Alpha::Particle::EmitDistribution;
//    using Mesh = ::Alpha::Renderer::MeshComponent;
//
//    constexpr UInt32 invalid_index = 0xFFFFFFFFu;
//
//    // 形状の分布を確認するときは false。
//    constexpr bool move_particles = false;
//
//    // false でも MeshComponent を付け、RenderScene と頂点更新に参加させる。
//    // true にすると同じ位置に生成元の球・立方体も表示する。
//    constexpr bool show_source_meshes = false;
//
//    // キャラクターは元のマテリアルで表示する。false なら粒子だけで形を確認する。
//    // 非表示でもメッシュのインスタンス登録と頂点更新は行う。
//    constexpr bool show_character_meshes = false;
//
//    constexpr float character_scale = 0.7f;
//    constexpr float character_emission_rate = 300.0f;
//
//    auto& resources = RenderResourceManager::Get();
//    auto& assets = AssetManager::Get();
//
//    // MaterialOverride の入力には CPU 側のテクスチャパレット index を使う。
//    const UInt32 white_texture = resources.RegisterTexture(assets.GetWhiteTexture());
//    const UInt32 normal_texture = resources.RegisterTexture(assets.GetNormalTexture());
//    const UInt32 black_texture = resources.RegisterTexture(assets.GetBlackTexture());
//
//    auto create_solid_mesh = [&](
//        const wchar_t* name,
//        const Vector3& position,
//        const Vector3& scale,
//        const Color3& color)
//    {
//        SceneObject& object = scene.CreateObject(name);
//        object.AddComponent<Mesh>().mesh_index = cube_index;
//
//        auto& transform = object.GetComponent<TransformComponent>();
//        transform.SetLocalPosition(position);
//        transform.SetLocalScale(scale);
//
//        auto& material = object.AddComponent<MaterialOverrideComponent>();
//        material.materials = resources.GetDefaultMaterials(cube_index);
//
//        for (UInt32 i = 0; i < static_cast<UInt32>(material.materials.size()); ++i)
//        {
//            auto& data = material.materials[i];
//
//            data.base_color_tex_idx = white_texture;
//            data.normal_tex_idx = normal_texture;
//            data.metallic_roughness_tex_idx = white_texture;
//            data.emissive_tex_idx = black_texture;
//
//            material.SetBaseColor(i, color);
//            material.SetRoughness(i, 0.9f);
//            material.SetMetallic(i, 0.0f);
//        }
//    };
//
//    create_solid_mesh(
//        L"ShapeTest_Ground",
//        Vector3(0.0f, -0.35f, 1.0f),
//        Vector3(72.0f, 0.6f, 14.0f),
//        Color3(0.055f, 0.065f, 0.085f));
//
//    create_solid_mesh(
//        L"ShapeTest_Backdrop",
//        Vector3(0.0f, 8.5f, 5.0f),
//        Vector3(71.0f, 19.0f, 0.3f),
//        Color3(0.012f, 0.018f, 0.028f));
//
//    struct Preset
//    {
//        const wchar_t* emitter_name;
//        const wchar_t* pedestal_name;
//
//        Shape shape;
//        Distribution distribution;
//
//        UInt32 column;
//        UInt32 row;
//
//        UInt32 source_mesh;
//
//        Vector3 scale;
//
//        float emission_rate;
//        float particle_size;
//    };
//
//
//    const Preset presets[] =
//    {
//        // =====================================================================
//        // Row 0
//        // =====================================================================
//
//        {
//            L"01_Box_Surface",
//            L"01_Pedestal",
//            Shape::Box,
//            Distribution::Surface,
//            0, 0,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            300.0f,
//            0.045f
//        },
//        {
//            L"02_Sphere_Surface",
//            L"02_Pedestal",
//            Shape::Sphere,
//            Distribution::Surface,
//            1, 0,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            300.0f,
//            0.045f
//        },
//        {
//            L"03_Hemisphere_Surface",
//            L"03_Pedestal",
//            Shape::Hemisphere,
//            Distribution::Surface,
//            2, 0,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            300.0f,
//            0.045f
//        },
//        {
//            L"04_Cylinder_Surface",
//            L"04_Pedestal",
//            Shape::Cylinder,
//            Distribution::Surface,
//            3, 0,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            300.0f,
//            0.045f
//        },
//        {
//            L"05_MeshSphere_Surface",
//            L"05_Pedestal",
//            Shape::Mesh,
//            Distribution::Surface,
//            4, 0,
//            sphere_index,
//            Vector3(1.4f, 1.4f, 1.4f),
//            300.0f,
//            0.045f
//        },
//
//        // 追加
//        {
//            L"20_Box_Vertex",
//            L"20_Pedestal",
//            Shape::Box,
//            Distribution::Vertex,
//            5, 0,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            30.0f,
//            0.075f
//        },
//        {
//            L"21_Box_Edge",
//            L"21_Pedestal",
//            Shape::Box,
//            Distribution::Edge,
//            6, 0,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            200.0f,
//            0.050f
//        },
//        {
//            L"22_Hemisphere_Edge",
//            L"22_Pedestal",
//            Shape::Hemisphere,
//            Distribution::Edge,
//            7, 0,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            200.0f,
//            0.050f
//        },
//        {
//            L"23_Cone_Edge",
//            L"23_Pedestal",
//            Shape::Cone,
//            Distribution::Edge,
//            8, 0,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            200.0f,
//            0.050f
//        },
//
//        // =====================================================================
//        // Row 1
//        // =====================================================================
//
//        {
//            L"06_Box_Volume",
//            L"06_Pedestal",
//            Shape::Box,
//            Distribution::Volume,
//            0, 1,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            300.0f,
//            0.045f
//        },
//        {
//            L"07_Sphere_Volume",
//            L"07_Pedestal",
//            Shape::Sphere,
//            Distribution::Volume,
//            1, 1,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            300.0f,
//            0.045f
//        },
//        {
//            L"08_Hemisphere_Volume",
//            L"08_Pedestal",
//            Shape::Hemisphere,
//            Distribution::Volume,
//            2, 1,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            300.0f,
//            0.045f
//        },
//        {
//            L"09_Cylinder_Volume",
//            L"09_Pedestal",
//            Shape::Cylinder,
//            Distribution::Volume,
//            3, 1,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            300.0f,
//            0.045f
//        },
//        {
//            L"10_Cone_Volume",
//            L"10_Pedestal",
//            Shape::Cone,
//            Distribution::Volume,
//            4, 1,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            300.0f,
//            0.045f
//        },
//
//        // 追加
//        {
//            L"24_Cone_Surface",
//            L"24_Pedestal",
//            Shape::Cone,
//            Distribution::Surface,
//            5, 1,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            300.0f,
//            0.045f
//        },
//        {
//            L"25_Cylinder_Edge",
//            L"25_Pedestal",
//            Shape::Cylinder,
//            Distribution::Edge,
//            6, 1,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            200.0f,
//            0.050f
//        },
//        {
//            L"26_Circle_Edge",
//            L"26_Pedestal",
//            Shape::Circle,
//            Distribution::Edge,
//            7, 1,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            200.0f,
//            0.050f
//        },
//        {
//            L"27_Rectangle_Vertex",
//            L"27_Pedestal",
//            Shape::Rectangle,
//            Distribution::Vertex,
//            8, 1,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            30.0f,
//            0.075f
//        },
//
//        // =====================================================================
//        // Row 2
//        // =====================================================================
//
//        {
//            L"11_Point_Center",
//            L"11_Pedestal",
//            Shape::Point,
//            Distribution::Center,
//            0, 2,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            8.0f,
//            0.10f
//        },
//        {
//            L"12_Circle_Surface",
//            L"12_Pedestal",
//            Shape::Circle,
//            Distribution::Surface,
//            1, 2,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            200.0f,
//            0.045f
//        },
//        {
//            L"13_MeshSphereA_Vertex",
//            L"13_Pedestal",
//            Shape::Mesh,
//            Distribution::Vertex,
//            2, 2,
//            sphere_index,
//            Vector3(1.4f, 1.4f, 1.4f),
//            300.0f,
//            0.055f
//        },
//        {
//            L"14_MeshSphereB_Vertex",
//            L"14_Pedestal",
//            Shape::Mesh,
//            Distribution::Vertex,
//            3, 2,
//            sphere_index,
//            Vector3(0.9f, 1.7f, 1.2f),
//            300.0f,
//            0.055f
//        },
//        {
//            L"15_MeshCube_Vertex",
//            L"15_Pedestal",
//            Shape::Mesh,
//            Distribution::Vertex,
//            4, 2,
//            cube_index,
//            Vector3(1.6f, 1.6f, 1.6f),
//            40.0f,
//            0.085f
//        },
//
//        // 追加
//        {
//            L"28_Rectangle_Edge",
//            L"28_Pedestal",
//            Shape::Rectangle,
//            Distribution::Edge,
//            5, 2,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            200.0f,
//            0.050f
//        },
//        {
//            L"29_Rectangle_Surface",
//            L"29_Pedestal",
//            Shape::Rectangle,
//            Distribution::Surface,
//            6, 2,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            250.0f,
//            0.045f
//        },
//        {
//            L"30_Line_Vertex",
//            L"30_Pedestal",
//            Shape::Line,
//            Distribution::Vertex,
//            7, 2,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            20.0f,
//            0.075f
//        },
//        {
//            L"31_Line_Edge",
//            L"31_Pedestal",
//            Shape::Line,
//            Distribution::Edge,
//            8, 2,
//            invalid_index,
//            Vector3(1.0f, 1.0f, 1.0f),
//            150.0f,
//            0.050f
//        },
//    };
//
//    const float4 row_colors[] =
//    {
//        float4(1.0f, 0.48f, 0.08f, 1.0f),
//        float4(0.08f, 0.65f, 1.0f, 1.0f),
//        float4(0.30f, 1.0f, 0.45f, 1.0f),
//    };
//
//    const Color3 pedestal_colors[] =
//    {
//        Color3(0.18f, 0.065f, 0.015f),
//        Color3(0.015f, 0.075f, 0.18f),
//        Color3(0.025f, 0.14f, 0.045f),
//    };
//
//    // =========================================================================
//    // Shape
//    //
//    // 以前の
//    //
//    //   source_shape = ...
//    //   distribution = ...
//    //
//    // ではなく、Mesh 以外は SetShapeXXX() を通す。
//    // =========================================================================
//
//    auto configure_shape = [&](
//        Emitter& emitter,
//        Shape shape,
//        Distribution distribution)
//    {
//        switch (shape)
//        {
//        case Shape::Point:
//        {
//            emitter.SetShapePoint();
//            break;
//        }
//
//        case Shape::Box:
//        {
//            // 以前の box_half_size = (1.2, 1.0, 0.9) 相当。
//            // SetShapeBoxXXX() は全幅 size を受け取る。
//            const float3 size(2.4f, 2.0f, 1.8f);
//
//            switch (distribution)
//            {
//            case Distribution::Vertex:
//                emitter.SetShapeBoxVertex(size);
//                break;
//
//            case Distribution::Edge:
//                emitter.SetShapeBoxEdge(size);
//                break;
//
//            case Distribution::Surface:
//                emitter.SetShapeBoxSurface(size);
//                break;
//
//            case Distribution::Volume:
//                emitter.SetShapeBoxVolume(size);
//                break;
//
//            default:
//                break;
//            }
//
//            break;
//        }
//
//        case Shape::Sphere:
//        {
//            constexpr float radius = 1.25f;
//
//            switch (distribution)
//            {
//            case Distribution::Surface:
//                emitter.SetShapeSphereSurface(radius);
//                break;
//
//            case Distribution::Volume:
//                emitter.SetShapeSphereVolume(radius);
//                break;
//
//            default:
//                break;
//            }
//
//            break;
//        }
//
//        case Shape::Hemisphere:
//        {
//            constexpr float radius = 1.25f;
//
//            switch (distribution)
//            {
//            case Distribution::Edge:
//                //emitter.SetShapeHemisphereEdge(radius);
//                break;
//
//            case Distribution::Surface:
//                emitter.SetShapeHemisphereSurface(radius);
//                break;
//
//            case Distribution::Volume:
//                emitter.SetShapeHemisphereVolume(radius);
//                break;
//
//            default:
//                break;
//            }
//
//            break;
//        }
//
//        case Shape::Cone:
//        {
//            constexpr float radius = 1.25f;
//            constexpr float height = 2.5f;
//
//            // radius=1.25 / height=2.5 と同じ形になる全開き角。
//            constexpr float angle_degrees = 53.130102f;
//
//            switch (distribution)
//            {
//            case Distribution::Edge:
//                //emitter.SetShapeConeEdge(radius, height);
//                break;
//
//            case Distribution::Surface:
//                emitter.SetShapeConeSurface(radius, height);
//                break;
//
//            case Distribution::Volume:
//                emitter.SetShapeConeVolume(radius, height);
//                break;
//
//            default:
//                break;
//            }
//
//            break;
//        }
//
//        case Shape::Cylinder:
//        {
//            constexpr float radius = 1.25f;
//            constexpr float height = 2.5f;
//
//            switch (distribution)
//            {
//            case Distribution::Edge:
//                emitter.SetShapeCylinderEdge(radius, height);
//                break;
//
//            case Distribution::Surface:
//                emitter.SetShapeCylinderSurface(radius, height);
//                break;
//
//            case Distribution::Volume:
//                emitter.SetShapeCylinderVolume(radius, height);
//                break;
//
//            default:
//                break;
//            }
//
//            break;
//        }
//
//        case Shape::Circle:
//        {
//            constexpr float radius = 1.25f;
//
//            switch (distribution)
//            {
//            case Distribution::Edge:
//                emitter.SetShapeCircleEdge(radius);
//                break;
//
//            case Distribution::Surface:
//                emitter.SetShapeCircleSurface(radius);
//                break;
//
//            default:
//                break;
//            }
//
//            break;
//        }
//
//        case Shape::Rectangle:
//        {
//            // 以前の rectangle_half_size = (1.4, 0.8) 相当。
//            // setter は全幅 size。
//            const float2 size(2.8f, 1.6f);
//
//            switch (distribution)
//            {
//            case Distribution::Vertex:
//                emitter.SetShapeRectangleVertex(size);
//                break;
//
//            case Distribution::Edge:
//                emitter.SetShapeRectangleEdge(size);
//                break;
//
//            case Distribution::Surface:
//                emitter.SetShapeRectangleSurface(size);
//                break;
//
//            default:
//                break;
//            }
//
//            break;
//        }
//
//        case Shape::Line:
//        {
//            constexpr float length = 2.8f;
//
//            switch (distribution)
//            {
//            case Distribution::Vertex:
//                emitter.SetShapeLineVertex(length);
//                break;
//
//            case Distribution::Edge:
//                emitter.SetShapeLineEdge(length);
//                break;
//
//            default:
//                break;
//            }
//
//            break;
//        }
//
//        case Shape::Mesh:
//        {
//            // Mesh には現在 SetShapeMeshXXX() が無いので、
//            // ここだけ従来通り直接設定。
//            emitter.source_shape = Shape::Mesh;
//            emitter.distribution = distribution;
//            break;
//        }
//
//        default:
//            break;
//        }
//    };
//
//    auto configure_emitter = [&](
//        Emitter& emitter,
//        Shape shape,
//        Distribution distribution,
//        float rate,
//        float size,
//        const float4& color,
//        UInt32 seed)
//    {
//        emitter.max_particle_count = 1000;
//
//        emitter.emission_rate = rate;
//        emitter.lifetime = 2.0f;
//        emitter.duration = 3600.0f;
//
//        emitter.initial_velocity = move_particles
//            ? float3(0.0f, 0.4f, 0.0f)
//            : float3(0.0f, 0.0f, 0.0f);
//
//        emitter.velocity_random = move_particles
//            ? float3(0.12f, 0.12f, 0.12f)
//            : float3(0.0f, 0.0f, 0.0f);
//
//        emitter.start_color = color;
//        emitter.end_color = color;
//
//        emitter.start_size = size;
//        emitter.end_size = size;
//
//        emitter.random_seed = seed;
//        emitter.spawn_accumulator = 0.0f;
//
//        // Shape 設定は setter 経由。
//        configure_shape(
//            emitter,
//            shape,
//            distribution);
//
//        emitter.Start();
//    };
//
//    // =========================================================================
//    // Shape Emitters
//    // =========================================================================
//
//    UInt32 preset_index = 0;
//
//    for (const auto& preset : presets)
//    {
//        // 5列 -> 9列へ拡張。
//        // -24, -18, -12, -6, 0, 6, 12, 18, 24
//        const float x =
//            -24.0f +
//            6.0f * static_cast<float>(preset.column);
//
//        const float y =
//            14.4f -
//            5.8f * static_cast<float>(preset.row);
//
//        const Vector3 position(x, y, 0.0f);
//
//        create_solid_mesh(
//            preset.pedestal_name,
//            Vector3(x, y - 2.3f, 0.0f),
//            Vector3(3.6f, 0.14f, 3.6f),
//            pedestal_colors[preset.row]);
//
//        SceneObject& object =
//            scene.CreateObject(preset.emitter_name);
//
//        auto& transform =
//            object.GetComponent<TransformComponent>();
//
//        transform.SetLocalPosition(position);
//        transform.SetLocalScale(preset.scale);
//
//        // =====================================================================
//        // Mesh
//        //
//        // sphere_index / cube_index は引数でもらった値を
//        // source_mesh -> MeshComponent::mesh_index にそのまま渡す。
//        // =====================================================================
//
//        if (preset.shape == Shape::Mesh)
//        {
//            auto& mesh = object.AddComponent<Mesh>();
//
//            mesh.mesh_index = preset.source_mesh;
//            mesh.visible = show_source_meshes;
//        }
//
//        auto& emitter = object.AddComponent<Emitter>();
//
//        configure_emitter(
//            emitter,
//            preset.shape,
//            preset.distribution,
//            preset.emission_rate,
//            preset.particle_size,
//            row_colors[preset.row],
//            1001u + preset_index * 97u);
//
//        ++preset_index;
//    }
//
//    // =========================================================================
//    // Character Mesh Emitters
//    // =========================================================================
//
//    struct CharacterPreset
//    {
//        const wchar_t* emitter_name;
//        const wchar_t* pedestal_name;
//
//        UInt32 mesh_index;
//
//        float x;
//
//        Distribution distribution;
//
//        float4 particle_color;
//        Color3 pedestal_color;
//
//        UInt32 random_seed;
//    };
//
//    const CharacterPreset characters[] =
//    {
//        {
//            L"18_Akari_Vertex",
//            L"18_Akari_Pedestal",
//
//            // 引数の値をそのまま使う。
//            akari_index,
//
//            -32.0f,
//
//            Distribution::Vertex,
//
//            float4(1.0f, 0.30f, 0.06f, 1.0f),
//            Color3(0.18f, 0.05f, 0.015f),
//
//            8101u,
//        },
//
//        {
//            L"19_Patchouli_Vertex",
//            L"19_Patchouli_Pedestal",
//
//            // 引数の値をそのまま使う。
//            patchouli_index,
//
//            32.0f,
//
//            Distribution::Vertex,
//
//            float4(0.75f, 0.25f, 1.0f, 1.0f),
//            Color3(0.10f, 0.025f, 0.18f),
//
//            8201u,
//        },
//    };
//
//    for (const auto& preset : characters)
//    {
//        create_solid_mesh(
//            preset.pedestal_name,
//            Vector3(preset.x, 0.25f, 0.0f),
//            Vector3(8.0f, 0.5f, 8.0f),
//            preset.pedestal_color);
//
//        SceneObject& object =
//            scene.CreateObject(preset.emitter_name);
//
//        auto& transform =
//            object.GetComponent<TransformComponent>();
//
//        transform.SetLocalPosition(
//            Vector3(preset.x, 0.5f, 0.0f));
//
//        transform.SetLocalScale(
//            Vector3(
//            character_scale,
//            character_scale,
//            character_scale));
//
//        auto& mesh = object.AddComponent<Mesh>();
//
//        // akari_index / patchouli_index をそのまま渡す。
//        mesh.mesh_index = preset.mesh_index;
//        mesh.visible = show_character_meshes;
//
//        // MaterialOverrideComponent を付けず、
//        // 登録済み PMX のマテリアルを使用する。
//        auto& emitter = object.AddComponent<Emitter>();
//
//        configure_emitter(
//            emitter,
//            Shape::Mesh,
//            preset.distribution,
//            character_emission_rate,
//            0.065f,
//            preset.particle_color,
//            preset.random_seed);
//    }
//
//    // =========================================================================
//    // Light
//    // =========================================================================
//
//    {
//        SceneObject& object =
//            scene.CreateObject(L"ShapeTest_Light");
//
//        auto& light =
//            object.AddComponent<LightComponent>();
//
//        light.type = 2;
//
//        light.direction =
//            VectorNormalize(
//            Vector3(-0.3f, -1.0f, 0.4f));
//
//        light.SetColor(
//            float3(0.95f, 0.97f, 1.0f));
//
//        light.SetIntensity(3.0f);
//    }
//
//
//    {
//        SceneObject& object =
//            scene.CreateObject(L"MainCamera");
//
//        auto& transform =
//            object.GetComponent<TransformComponent>();
//
//        transform.SetLocalPosition(
//            Vector3(0.0f, 8.8f, -54.0f));
//
//        // 方向を渡す LookAt。
//        transform.LookAt(
//            Vector3(0.0f, -0.023f, 1.0f));
//
//        object.AddComponent<CameraComponent>();
//    }
//}