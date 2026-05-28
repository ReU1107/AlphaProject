#pragma once
#include "Core/Types.h"
#include "Core/Subsystem.h"

namespace Alpha
{
	struct RenderContext;

	class RenderCore;
	class RingBuffer;
	class Shader;
	class PipelineLayout;
	class GraphicsPipeline;

	struct LineVertex
	{
		float3 position{ float3::Zero };
		color4 color{ color4::White };
	};

	class DebugRenderer : public Subsystem<DebugRenderer>
	{
		struct DebugLine
		{
			Vector3 start;
			Vector3 end;
			Color4 color;
			float thickness;	// 線の太さ
		};
	private:
		TArray<DebugLine> m_lines{};
		TUnique<RingBuffer> m_line_buffer{};

		TUnique<Shader> m_line_vs{};
		TUnique<Shader> m_line_ps{};
		TUnique<PipelineLayout> m_line_pipeline_layout{};
		TUnique<GraphicsPipeline> m_line_pipelie{};

	public:
		struct Args
		{
			RenderCore* render_core{ nullptr };
			uint32_t max_line_num{ 1024 };

		};
		bool Initialize(const Args& args);
		void Finalize();

		void DrawLine(const Vector3& start, const Vector3& end, const Color4& color, float thickness = 1.0f);
		void DrawGizmo(const Vector3& position, const Vector3& dir, const Color4& color, float thickness = 1.0f);
		void DrawGizmo(const Vector3& position, const Vector3& dir, float thickness = 1.0f);
		void DrawSphere(const Vector3& center, float radius, const Color4& color, float thickness = 1.0f);
		void DrawBox(const Vector3& center, const Vector3& extents, const Color4& color, float thickness = 1.0f);

		void Flush(RenderContext& ctx);

	};
}