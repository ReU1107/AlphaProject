#include "GraphicsPipeline.h"
#include "PipelineLayout.h"
#include "RenderHardwareInterface.h"
#include "Shader.h"
#include "RHI/RHIGraphicsPipeline.h"

namespace Alpha
{
	GraphicsPipeline::GraphicsPipeline() = default;
	GraphicsPipeline::~GraphicsPipeline() = default;

	void GraphicsPipeline::Initialize(const GraphicsPipelineArgs& args)
	{
        RHIGraphicsPipelineDesc desc = {};
        desc.layout = args.pipeline_layout->GetRHI();
        desc.vertex_layout = args.vertex_layout.ToRHIDesc();

        if (args.vertex_shader)
            desc.vertex_shader = args.vertex_shader->GetRHI();

        if (args.hull_shader)
            desc.hull_shader = args.hull_shader->GetRHI();

        if (args.domain_shader)
            desc.domain_shader = args.domain_shader->GetRHI();

        if (args.geometry_shader)
            desc.geometry_shader = args.geometry_shader->GetRHI();

        if (args.pixel_shader)
            desc.pixel_shader = args.pixel_shader->GetRHI();

        if (args.mesh_shader)
            desc.mesh_shader = args.mesh_shader->GetRHI();

        if (args.task_shader)
            desc.task_shader = args.task_shader->GetRHI();

        for (UInt32 i = 0; i < args.render_target_count; ++i)
        {
            desc.render_targets[i] = args.render_targets[i];
            desc.blend_states[i] = args.blend_states[i];
        }
        desc.render_target_count = args.render_target_count;

        desc.primitive_topology = args.primitive_topology;

        desc.depth_format = args.depth_format;
        desc.depth_enable = args.depth_enable;
        desc.depth_write_enable = args.depth_write_enable;
        desc.depth_compare_op = args.depth_compare_op;

        desc.cull_mode = args.cull_mode;
        desc.fill_mode = args.fill_mode;
        desc.rasterizer_state = args.rasterizer_state;

        desc.alpha_to_coverage_enable = args.alpha_to_coverage_enable;
        desc.independent_blend_enable = args.independent_blend_enable;

        m_rhi = args.rhi->CreateGraphicsPipeline(desc);

        m_layout = args.pipeline_layout;
        m_primitive_topology = args.primitive_topology;

	}

	void GraphicsPipeline::Finalize()
	{
	}

	RHIGraphicsPipeline* GraphicsPipeline::GetRHI()
	{
		return m_rhi.get();
	}

	PipelineLayout* GraphicsPipeline::GetPipelineLayout()
	{
		return m_layout;
	}
}
