#pragma once
#include "Core/Common.h"
#include "Core/Math/AxisAlignedBoundingBox.h"
#include "Definitions.h"
#include <entt/entt.hpp>

namespace Alpha
{
	class RenderCore;
	class TransientBufferAllocator;
	class VertexComputePass;
	class PipelineLayout;

	struct RenderContext;

	class RenderScene
	{
	private:
		TUnique<TransientBufferAllocator> m_instance_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_vert_to_inst_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_instance_tlas_allocator{ nullptr };

		TUnique<TransientBufferAllocator> m_scratch_tlas_allocator{ nullptr };

		TUnique<TransientBufferAllocator> m_result_tlas_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_result_blas_allocator{ nullptr };

		TUnique<TransientBufferAllocator> m_dynamic_vertex_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_override_material_allocator{ nullptr };

		TUnique<TransientBufferAllocator> m_light_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_volume_allocator{ nullptr };
		TUnique<TransientBufferAllocator> m_procedural_primitive_allocator{ nullptr };

		TArray<LightData> m_lights{};
		TArray<InstanceData> m_instances{};
		TArray<VolumeMaterial> m_volume_materials{};	
		TArray<AxisAlignedBoundingBox> m_procedural_primitives{};	// レイトレのprocedural用

		TUnique<VertexComputePass> m_vertex_compute_pass{ nullptr };

	public:

		~RenderScene();
		struct Args
		{
			RenderCore* render_core{ nullptr };
			PipelineLayout* layout{ nullptr };
		};
		bool Initialize(const Args& args);
		void Finalize();

		void Build(entt::registry& registry, RenderContext& ctx);

		DynamicBufferIndices GetBufferIndices() const;

		UInt32 GetLightCount() const { return static_cast<UInt32>(m_lights.size()); }
		UInt32 GetInstanceCount() const { return static_cast<UInt32>(m_instances.size()); }

	};
}

/*
* VolumeRenderに使用するやつらは
* Scene情報のシリアライズとかができたらRenderResourceManagerに移動させる
* m_volume_materials
* m_volume_allocator
*/