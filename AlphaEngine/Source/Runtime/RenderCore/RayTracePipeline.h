#pragma once
#include "Core/Common.h"
#include "Definitions.h"

namespace Alpha
{
	class PersistentBufferAllocator;

	class RayTracePipeline
	{
	private:
		TUnique<RHIRayTracePipeline> m_rhi{ nullptr };
		PipelineLayout* m_global_layout{ nullptr };
		TUnique<PersistentBufferAllocator> m_shader_table_allocator{ nullptr };
		
		ShaderTable m_ray_gen_table{};
		ShaderTable m_miss_table{};
		ShaderTable m_hit_group_table{};

	public:
		RayTracePipeline();
		~RayTracePipeline();


		void Initialize(const RayTracePipelineArgs& args);

		void Finalize() {};

		RHIRayTracePipeline* GetRHI() const;

		PipelineLayout* GetPipelineLayout() const { return m_global_layout; }

		Buffer* GetShaderTableBuffer() const;

		const ShaderTable& GetRayGenerateTable() const { return m_ray_gen_table; }
		const ShaderTable& GetMissTable() const { return m_miss_table; }
		const ShaderTable& GetHitGroupTable() const { return m_hit_group_table; }

	};

}