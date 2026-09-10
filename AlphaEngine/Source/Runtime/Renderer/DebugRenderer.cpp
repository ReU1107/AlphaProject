#include "DebugRenderer.h"
#include "RenderCore/Module.h"

#include "Definitions.h"

#include "Core/Math/MathF.h"


namespace Alpha::Renderer
{ 
	bool DebugRenderer::Initialize(const Args& args)
	{

		m_line_buffer = std::make_unique<TransientBufferAllocator>();
		{
			TransientBufferAllocator::Args desc = {
				.render_core = args.render_core,
				.stride = sizeof(LineVertex),
				.size = sizeof(LineVertex) * args.max_line_num * 2,
				.alignment = 4,
			};
			m_line_buffer->Initialize(desc);
		}

		m_lines.reserve(args.max_line_num);

		auto vs = args.render_core->CreateShader(L"Asset/LineVS.hlsl", ERHIShaderType::Vertex);
		auto ps = args.render_core->CreateShader(L"Asset/LinePS.hlsl", ERHIShaderType::Pixel);

		auto pipeline_layout = args.render_core->CreatePipelineLayout([](PipelineLayout& layout) {
				layout.AddRootCBV(1);
				layout.SetFlags(ERHIPipelineLayoutFlags::InputAssembler);
			}
		);

		m_line_pipelie = args.render_core->CreateGraphicsPipeline(
			{
				.vertex_layout = VertexLayout()
					.AddElement(L"POSITION", EVertexElementType::Float3, 0)
					.AddElement(L"COLOR", EVertexElementType::Float4, 0),
				.pipeline_layout = pipeline_layout,
				.vertex_shader = vs,
				.pixel_shader = ps,
				.render_target_count = 1,
				.render_targets = {
					ERHIResourceFormat::RGBAFloat
				},
				.cull_mode = ERHICullMode::None,
				.primitive_topology = ERHIPrimitiveTopology::LineList,
			}
		);

		return true;
	}

	void DebugRenderer::Finalize()
	{

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

	void DebugRenderer::DrawPlane(const Plane& plane, const Color4& color, float thickness)
	{

	}

	void DebugRenderer::DrawFrustum(const Frustum& frustum, const Color4& color, float thickness)
	{
		const Plane& pLeft = frustum.planes[+EFrustumSide::Left];
		const Plane& pRight = frustum.planes[+EFrustumSide::Right];
		const Plane& pTop = frustum.planes[+EFrustumSide::Top];
		const Plane& pBottom = frustum.planes[+EFrustumSide::Bottom];
		const Plane& pNear = frustum.planes[+EFrustumSide::Near];
		const Plane& pFar = frustum.planes[+EFrustumSide::Far];

		auto GetIntersection = [](const Plane& p1, const Plane& p2, const Plane& p3) -> Vector3 {
			Vector3 n1 = p1.normal;
			Vector3 n2 = p2.normal;
			Vector3 n3 = p3.normal;

			// SignedDistance の定義が + plane.distance なので符号を反転
			float d1 = -p1.distance;
			float d2 = -p2.distance;
			float d3 = -p3.distance;

			float denom = VectorDot(n1, Vector3Cross(n2, n3));
			if (std::abs(denom) < 0.0001f) return Vector3(0.0f, 0.0f, 0.0f);

			Vector3 num = (Vector3Cross(n2, n3) * d1) +
				(Vector3Cross(n3, n1) * d2) +
				(Vector3Cross(n1, n2) * d3);
			return num / denom;
		};

		Vector3 nlt = GetIntersection(pNear, pLeft, pTop);
		Vector3 nrt = GetIntersection(pNear, pRight, pTop);
		Vector3 nlb = GetIntersection(pNear, pLeft, pBottom);
		Vector3 nrb = GetIntersection(pNear, pRight, pBottom);

		Vector3 flt = GetIntersection(pFar, pLeft, pTop);
		Vector3 frt = GetIntersection(pFar, pRight, pTop);
		Vector3 flb = GetIntersection(pFar, pLeft, pBottom);
		Vector3 frb = GetIntersection(pFar, pRight, pBottom);

		DrawLine(nlt, nrt, color, thickness);
		DrawLine(nrt, nrb, color, thickness);
		DrawLine(nrb, nlb, color, thickness);
		DrawLine(nlb, nlt, color, thickness);

		DrawLine(flt, frt, color, thickness);
		DrawLine(frt, frb, color, thickness);
		DrawLine(frb, flb, color, thickness);
		DrawLine(flb, flt, color, thickness);

		DrawLine(nlt, flt, color, thickness);
		DrawLine(nrt, frt, color, thickness);
		DrawLine(nlb, flb, color, thickness);
		DrawLine(nrb, frb, color, thickness);
	}

	void DebugRenderer::Flush(RenderContext& ctx)
	{
		if (m_lines.empty())
			return;

		m_line_buffer->Reset(ctx.current_frame_index);

		uint64_t vertex_num = m_lines.size() * 2;
		uint64_t vertex_size = sizeof(LineVertex) * vertex_num;

		uint64_t offset = 0;
		auto vertices = reinterpret_cast<LineVertex*>(m_line_buffer->Allocate(vertex_size, &offset));


		for (auto& line : m_lines)
		{
			vertices->position = line.start;
			vertices->color = line.color;
			vertices++;

			vertices->position = line.end;
			vertices->color = line.color;
			vertices++;
		}

		ctx.command_list->SetGraphicsPipeline(m_line_pipelie);
		
		ctx.command_list->SetGraphicsCBV(ctx.transient_buffer->GetBuffer(), ctx.current_camera_offset, 0);

		ctx.command_list->SetVertexBuffer(m_line_buffer->GetBuffer(), 0);
		ctx.command_list->Draw(static_cast<uint32_t>(vertex_num), 1, 0, 0);


		m_lines.clear();
	}


}