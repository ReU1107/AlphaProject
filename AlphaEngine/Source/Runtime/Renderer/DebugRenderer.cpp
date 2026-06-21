#include "DebugRenderer.h"
#include "RenderCore/RenderCore.h"
#include "RenderCore/CommandList.h"
#include "RenderCore/Shader.h"
#include "RenderCore/Buffer.h"
#include "RenderCore/Pipeline/PipelineLayout.h"
#include "RenderCore/Pipeline/GraphicsPipeline.h"

#include "Core/Math/MathF.h"

#include "RenderContext.h"

namespace Alpha
{ 
	bool DebugRenderer::Initialize(const Args& args)
	{
		m_line_buffer = args.render_core->CreateBuffer(sizeof(LineVertex), args.max_line_num * 2, EBufferPresetType::Upload);

		m_lines.reserve(args.max_line_num);

		m_line_vs = args.render_core->CreateShader(L"Asset/LineVS.hlsl", ERHIShaderFlags::Vertex);
		m_line_ps = args.render_core->CreateShader(L"Asset/LinePS.hlsl", ERHIShaderFlags::Pixel);

		m_line_pipeline_layout = args.render_core->CreatePipelineLayout([](PipelineLayout& layout) {
				layout.AddRootCBV(1);
			}
		);

		m_line_pipelie = args.render_core->CreateGraphicsPipeline(
			{
				.vertex_layout = VertexLayout()
					.AddElement(L"POSITION", EVertexElementType::Float3, 0)
					.AddElement(L"COLOR", EVertexElementType::Float4, 0),
				.pipeline_layout = m_line_pipeline_layout.get(),
				.vertex_shader = m_line_vs.get(),
				.pixel_shader = m_line_ps.get(),
				.render_target_count = 1,
				.render_targets = {
					args.render_core->GetBackBufferFormat(),
				},
				.cull_mode = ERHICullMode::None,
				.primitive_topology = ERHIPrimitiveTopology::LineList,
			}
		);

		return true;
	}

	void DebugRenderer::Finalize()
	{

		m_line_pipelie.reset();
		m_line_pipeline_layout.reset();
		m_line_ps.reset();
		m_line_vs.reset();
	}

	void DebugRenderer::DrawLine(const Vector3& start, const Vector3& end, const Color4& color, float thickness)
	{
		m_lines.push_back({ start, end, color, thickness });
	}

	void DebugRenderer::DrawGizmo(const Vector3& position, const Vector3& dir, const Color4& color, float thickness)
	{
		const Vector3 z_axis = dir;
		const Vector3 x_axis = VectorNormalize(Vector3Orthogonal(z_axis));
		const Vector3 y_axis = Vector3Cross(z_axis, x_axis);

		DrawLine(position, position + z_axis * 2.0f, color);
		DrawLine(position, position + x_axis * 2.0f, color);
		DrawLine(position, position + y_axis * 2.0f, color);
	}

	void DebugRenderer::DrawGizmo(const Vector3& position, const Vector3& dir, float thickness)
	{
		const Vector3 z_axis = dir;
		const Vector3 x_axis = VectorNormalize(Vector3Orthogonal(z_axis));
		const Vector3 y_axis = Vector3Cross(z_axis, x_axis);

		DrawLine(position, position + z_axis * 2.0f, Color4::Blue);
		DrawLine(position, position + x_axis * 2.0f, Color4::Red);
		DrawLine(position, position + y_axis * 2.0f, Color4::Green);
	}

	void DebugRenderer::DrawSphere(const Vector3& center, float radius, const Color4& color, float thickness)
	{
	}

	void DebugRenderer::DrawBox(const Vector3& center, const Vector3& extents, const Color4& color, float thickness)
	{
	}

	void DebugRenderer::Flush(RenderContext& ctx)
	{
		if (m_lines.empty())
			return;

		m_line_buffer->Reset();

		uint64_t vertex_num = m_lines.size() * 2;
		uint64_t vertex_size = sizeof(LineVertex) * vertex_num;

		uint64_t offset = 0;
		auto vertices = reinterpret_cast<LineVertex*>(m_line_buffer->Allocate(vertex_size, 4, &offset));


		for (auto& line : m_lines)
		{
			vertices->position = line.start;
			vertices->color = line.color;
			vertices++;

			vertices->position = line.end;
			vertices->color = line.color;
			vertices++;
		}

		ctx.command_list->SetGraphicsPipeline(m_line_pipelie.get());
		
		ctx.command_list->SetGraphicsCBV(ctx.transient_buffer, ctx.current_camera_offset, 0);

		ctx.command_list->SetVertexBuffer(m_line_buffer.get(), 0);
		ctx.command_list->Draw(static_cast<uint32_t>(vertex_num), 1, 0, 0);


		m_lines.clear();
	}


}