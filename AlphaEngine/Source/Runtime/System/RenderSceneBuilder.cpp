#include "RenderSceneBuilder.h"

#include "Renderer/RenderScene.h"

#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include "Animation/Module.h"

#include "Renderer/Pass/VertexComputePass.h"

#include "Spatial/TransformComponent.h"

#include "Core/Math/MathF.h"
#include "Core/Math/GeometryUtil.h"


namespace Alpha
{
	using namespace Renderer;
    using namespace Animation;

    RenderSceneBuilder::~RenderSceneBuilder() = default;


    bool RenderSceneBuilder::Initialize(const Args& args)
    {
        m_render_core = args.render_core;

        m_instances.reserve(10000);
        m_lights.reserve(100);
        m_volume_materials.reserve(100);
        m_procedural_primitives.reserve(1000);

        m_vertex_compute_pass = std::make_unique<VertexComputePass>();

        m_vertex_compute_pass->Initialize(
            {
                args.render_core,
                args.layout
            });

        return true;
    }


    void RenderSceneBuilder::Finalize()
    {
    }

    void RenderSceneBuilder::Build(entt::registry& registry, RenderContext& ctx, Renderer::RenderScene& render_scene)
    {
        m_instances.clear();
        m_lights.clear();
        m_volume_materials.clear();
        m_procedural_primitives.clear();

        const Frustum frustum = CreateFrustum(ctx.inv_view_proj, 1.0f, 0.0f);

        UInt64 vertex_count = BuildMeshes(registry, ctx, render_scene);

        BuildVolumes(registry, frustum, render_scene);

        BuildLights(registry, render_scene);
    

        if (!m_instances.empty())
        {
            render_scene.WriteInstances(m_instances.data(), static_cast<UInt32>(m_instances.size()));
        }
        if (!m_lights.empty())
        {
            render_scene.WriteLights(m_lights.data(), static_cast<UInt32>(m_lights.size()));
        }

        if (!m_volume_materials.empty())
        {
            render_scene.WriteVolumes(m_volume_materials.data(), static_cast<UInt32>(m_volume_materials.size()));
        }

        if (!m_procedural_primitives.empty())
        {
			render_scene.WriteProceduralPrimitives(m_procedural_primitives.data(), static_cast<UInt32>(m_procedural_primitives.size()));
        }

        m_vertex_compute_pass->Execute(ctx, render_scene.GetDynamicVertexAllocator()->GetBuffer(), vertex_count);

        BuildAccelerationStructures(ctx, render_scene);
    }

    UInt64 RenderSceneBuilder::BuildMeshes(entt::registry& registry, RenderContext& ctx, RenderScene& render_scene)
    {
        auto& resource_manager = RenderResourceManager::Get();

        auto buffers = resource_manager.GetBuffers();

        auto view = registry.view<MeshComponent, TransformComponent>();

        UInt64 vertex_count = 0;

        for (auto [entity, mesh_com, transform] : view.each())
        {
            mesh_com.render_instance_index = 0xFFFFFFFF;

            if (!mesh_com.enabled)
                continue;


            const UInt32 current_instance_id = static_cast<UInt32>(m_instances.size());

            InstanceData instance{};

            instance.world_matrix = transform.GetWorldMatrix();
            instance.mesh_offset = resource_manager.GetGPUMeshOffset(mesh_com.mesh_index);

            instance.visible = mesh_com.visible ? 1u : 0u;

            if (auto* material_com = registry.try_get<MaterialOverrideComponent>(entity))
            {
                TArray<MaterialData> gpu_materials = material_com->materials;

                ResolveMaterialTextures(gpu_materials);

				UInt64 material_byte_offset = render_scene.GetOverrideMaterialAllocator()->WriteData(gpu_materials.data(), gpu_materials.size());

                instance.override_material_element_offset = material_byte_offset / sizeof(MaterialData);
            }

            if (auto* anim_com = registry.try_get<AnimationComponent>(entity))
            {
                const auto& palette = anim_com->m_pose->GetMatrixPallet();

                if (!palette.empty())
                {
                    const UInt64 palette_byte_offset = render_scene.GetMatrixPaletteAllocator()->WriteData(palette.data(), palette.size());

                    instance.matrix_palette_offset = palette_byte_offset / sizeof(Matrix4x4);
                }

            }

            const auto& mesh = resource_manager.GetMeshByIndex(mesh_com.mesh_index);

            UInt64 vertex_byte_offset = render_scene.GetDynamicVertexAllocator()->AllocateOffset(mesh.vertex_position_alloc.size);

            instance.vertex_element_offset = vertex_byte_offset / sizeof(Vector3);


            render_scene.GetVertToInstanceAllocator()->FillData(current_instance_id, mesh.vertex_count);


            vertex_count += mesh.vertex_count;

            UInt64 as_byte_offset = render_scene.GetBLASAllocator()->AllocateOffset(mesh.as.size);


            BLASRequest request =
            {
                .vertex_buffer = render_scene.GetDynamicVertexAllocator()->GetBuffer(),
                .vertex_offset = vertex_byte_offset,
                .index_buffer = buffers.index,
                .index_offset = mesh.index_alloc.offset,
                .result_buffer = render_scene.GetBLASAllocator()->GetBuffer(),
                .result_offset = as_byte_offset,
                .source_as_buffer = buffers.source_as,
                .source_as_offset = mesh.as.source_alloc.offset,
                .scratch_size = mesh.as.scratch_size,
                .vertex_count = mesh.vertex_count,
            };


            for (const auto& sub : mesh.submeshes)
            {
                request.geometries.push_back(
                    {
                        .index_byte_offset = sub.index_element_offset * sizeof(UInt32),
                        .index_count = sub.index_count,
                        .is_opaque = true,
                    });
            }


            m_render_core->GetASBuilder()->AddUpdateRequest(request);


            ASInstanceData as_instance{};
            as_instance.instance_id = current_instance_id;
            as_instance.blas_gpu_address = render_scene.GetBLASAllocator()->GetBuffer()->GetGPUAddress() + as_byte_offset;
            as_instance.transform = MatrixTranspose(instance.world_matrix);
            as_instance.hit_group_offset = 0;


            render_scene.GetTLASInstanceAllocator()->WriteData(&as_instance);


            m_instances.push_back(instance);
            mesh_com.render_instance_index = current_instance_id;

        }

        return vertex_count;
    }

    void RenderSceneBuilder::ResolveMaterialTextures(TArray<MaterialData>& materials)
    {
        auto& resource_manager = RenderResourceManager::Get();

        auto resolve = [&](UInt32& texture_index)
        {
            if (texture_index == 0xFFFFFFFF)
                return;

            auto& resource =resource_manager.GetTextureByIndex(texture_index);

            texture_index =resource.tex->GetSRVHandle().index;
        };


        for (auto& material : materials)
        {
            resolve(material.base_color_tex_idx);

            resolve(
                material.normal_tex_idx);

            resolve(
                material.metallic_roughness_tex_idx);

            resolve(
                material.emissive_tex_idx);
        }
    }

    void RenderSceneBuilder::BuildLights(entt::registry& registry,RenderScene& render_scene)
    {
		auto view = registry.view<LightComponent, TransformComponent>();


        for (auto [entity, light_com, transform] : view.each())
        {
            LightData light{};

            const Matrix4x4& world_matrix = transform.GetWorldMatrix();


            light.position = world_matrix._4();
            light.radius = light_com.GetRadius();
            light.color = light_com.GetColor();
            light.intensity = light_com.GetIntensity();
            light.type = light_com.type;

            if (light_com.type == 2)
            {
                // Directional
                light.direction = light_com.direction;
            }
            else if (light_com.type == 1)
            {
                // Spot
                light.direction = Matrix4x4Forward(world_matrix);
            }
            else
            {
                // Point
                light.direction = float3(0.0f, 0.0f, 0.0f);
            }
            light.inner_cone = light_com.GetInnerCone();
            light.outer_cone = light_com.GetOuterCone();


            m_lights.push_back(light);
        }
    }

    void RenderSceneBuilder::BuildVolumes( entt::registry& registry, const Frustum& frustum,  RenderScene& render_scene)
    {
		auto view = registry.view<VolumeComponent, TransformComponent>();

        for (auto [entity, volume_com, transform] : view.each())
        {
            AxisAlignedBoundingBox aabb = volume_com.GetAABB();

            aabb = TransformAABB(aabb, transform.GetLocalMatrix());


            if (!Intersect( frustum, aabb))
            {
                continue;
            }


            VolumeMaterial material{};

            material.min = aabb.min;

            material.max = aabb.max;

            material.density = volume_com.GetDensity();


            m_volume_materials.push_back(  material);


            // 将来Procedural BLASを戻すなら使う
            m_procedural_primitives.push_back(aabb);
        }

    }

    void RenderSceneBuilder::BuildAccelerationStructures(RenderContext& ctx, RenderScene& render_scene)
    {
        // BLASの更新
        m_render_core->GetASBuilder()->RecordUpdateCommands(ctx.command_list);

        ctx.command_list->UAVBarrier(render_scene.GetBLASAllocator()->GetBuffer());


        RHIBuildTLASDesc desc{};
        desc.instance_count = render_scene.GetTLASInstanceAllocator()->GetAllocateCount();
        desc.instance_desc_buffer = render_scene.GetTLASInstanceAllocator()->GetBuffer()->GetRHI();
        desc.result_as_buffer = render_scene.GetTLASAllocator()->GetBuffer()->GetRHI();
        desc.scratch_as_buffer = render_scene.GetTLASScratchAllocator()->GetBuffer()->GetRHI();


        ctx.command_list->BuildRaytracingAccelerationStructure(desc);

        ctx.command_list->UAVBarrier(render_scene.GetTLASAllocator()->GetBuffer());

    }

}
