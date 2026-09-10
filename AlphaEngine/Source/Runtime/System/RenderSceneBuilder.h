#pragma once

#include "Core/Common.h"
#include "Renderer/Definitions.h"
#include "Core/Math/GeometryUtil.h"

#include <entt/entt.hpp>

namespace Alpha::Renderer
{
    class RenderScene;
    class VertexComputePass;

    struct RenderContext;
}

namespace Alpha
{
    class RenderCore;
    class PipelineLayout;


    class RenderSceneBuilder
    {
    private:

        RenderCore* m_render_core{ nullptr };

        TUnique<Renderer::VertexComputePass> m_vertex_compute_pass{ nullptr };


        TArray<Renderer::InstanceData> m_instances{};
        TArray<Renderer::LightData> m_lights{};
        TArray<Renderer::VolumeMaterial> m_volume_materials{};
        TArray<AxisAlignedBoundingBox> m_procedural_primitives{};


    public:

        ~RenderSceneBuilder();

        struct Args
        {
            RenderCore* render_core{ nullptr };

            // VertexComputePassが今まだ共通Layoutを使うため
            PipelineLayout* layout{ nullptr };
        };

        bool Initialize(const Args& args);

        void Finalize();

        void Build(entt::registry& registry, Renderer::RenderContext& ctx, Renderer::RenderScene& render_scene);


    private:

        UInt64 BuildMeshes(entt::registry& registry, Renderer::RenderContext& ctx, Renderer::RenderScene& render_scene);

        void BuildVolumes(entt::registry& registry, const Frustum& frustum, Renderer::RenderScene& render_scene);
        void BuildLights(entt::registry& registry, Renderer::RenderScene& render_scene);

        void BuildAccelerationStructures(Renderer::RenderContext& ctx, Renderer::RenderScene& render_scene);
        
        void ResolveMaterialTextures(TArray<Renderer::MaterialData>& materials);
    };
}