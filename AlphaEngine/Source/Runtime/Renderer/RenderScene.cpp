#include "RenderScene.h"
#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include "Spatial/TransformComponent.h"


#include "Core/Math/MathF.h"
#include "Core/Math/GeometryUtil.h"

namespace Alpha::Renderer
{
	RenderScene::~RenderScene() = default;

	bool RenderScene::Initialize(const Args& args)
	{
		m_instance_allocator = CreateTransientAllocator(args.render_core, sizeof(InstanceData), 16_MB, EBufferPresetType::UploadStructured, L"Instance");
		m_vert_to_inst_allocator = CreateTransientAllocator(args.render_core, sizeof(UInt32), 32_MB, EBufferPresetType::UploadStructured, L"VertexToInstance");

		// 暫定
		TLASRequest request = {
			.instance_count = 2496
		};

		ASBuildSize build_size = args.render_core->PreBuildTLAS(request);

		m_instance_tlas_allocator = CreateTransientAllocator(args.render_core, sizeof(ASInstanceData), 16_MB, EBufferPresetType::UploadStructured, L"TLASInstance");

		m_scratch_tlas_allocator = CreateTransientAllocator(args.render_core, 256, build_size.scratch, EBufferPresetType::RWByteAddress, L"TLASScratch");

		m_result_tlas_allocator = CreateTransientAllocator(args.render_core, 256, build_size.result, EBufferPresetType::RTAccelStructure, L"TLASResult");
		m_result_blas_allocator = CreateTransientAllocator(args.render_core, 256, 256_MB, EBufferPresetType::RTAccelStructure, L"BLASResult");

		m_dynamic_vertex_allocator = CreateTransientAllocator(args.render_core, sizeof(Vector3), 64_MB, EBufferPresetType::RWStructured, L"DynamicVertex");
		m_override_material_allocator = CreateTransientAllocator(args.render_core, sizeof(MaterialData), 4_MB, EBufferPresetType::UploadStructured, L"OverrideMaterial");
		
		m_light_allocator = CreateTransientAllocator(args.render_core, sizeof(LightData), 4_MB, EBufferPresetType::UploadStructured, L"Light");
		m_procedural_primitive_allocator = CreateTransientAllocator(args.render_core, sizeof(AxisAlignedBoundingBox), 4_MB, EBufferPresetType::UploadStructured, L"ProceduralPrimitive");
		m_volume_allocator = CreateTransientAllocator(args.render_core, sizeof(VolumeMaterial), 4_MB, EBufferPresetType::UploadStructured, L"VolumeMaterial");
		
		
		m_matrix_palette_allocator = CreateTransientAllocator(args.render_core, sizeof(Matrix4x4), 12_MB, EBufferPresetType::UploadStructured, L"MatrixPalette");

		return true;
	}

	void RenderScene::Finalize()
	{
	}

	void RenderScene::Reset(UInt32 frame_index)
	{
		m_instance_allocator->Reset(frame_index);
		m_vert_to_inst_allocator->Reset(frame_index);

		m_dynamic_vertex_allocator->Reset(frame_index);

		m_result_tlas_allocator->Reset(frame_index);
		m_scratch_tlas_allocator->Reset(frame_index);
		m_result_blas_allocator->Reset(frame_index);

		m_override_material_allocator->Reset(frame_index);
		m_light_allocator->Reset(frame_index);

		m_procedural_primitive_allocator->Reset(frame_index);
		m_volume_allocator->Reset(frame_index);

		m_instance_tlas_allocator->Reset(frame_index);

		m_matrix_palette_allocator->Reset(frame_index);

		m_instance_count = 0;
		m_light_count = 0;
		m_volume_count = 0;
	}

	void RenderScene::WriteInstances(const InstanceData* data, UInt32 count)
	{
		m_instance_allocator->WriteData(data, count);

		m_instance_count = count;
	}

	void RenderScene::WriteLights(const LightData* data, UInt32 count)
	{
		m_light_allocator->WriteData(data, count);

		m_light_count = count;
	}

	void RenderScene::WriteVolumes(const VolumeMaterial* data, UInt32 count)
	{
		m_volume_allocator->WriteData(data, count);

		m_volume_count = count;
	}

	void RenderScene::WriteProceduralPrimitives(const AxisAlignedBoundingBox* data, UInt32 count)
	{
		m_procedural_primitive_allocator->WriteData(data, count);

	}

	DynamicBufferIndices RenderScene::GetBufferIndices() const
	{
		return DynamicBufferIndices{
			.instance = m_instance_allocator->GetSRVIndex(),
			.vertex_to_instance = m_vert_to_inst_allocator->GetSRVIndex(),
			.current_position_srv = m_dynamic_vertex_allocator->GetSRVIndex(),
			.current_position_uav = m_dynamic_vertex_allocator->GetUAVIndex(),
			.matrix_palette = m_matrix_palette_allocator->GetSRVIndex(),
			.override_material = m_override_material_allocator->GetSRVIndex(),
			.light = m_light_allocator->GetSRVIndex(),
			.volume = m_volume_allocator->GetSRVIndex(),
			.rt_structure = m_result_tlas_allocator->GetSRVIndex(),
		};
	}

	TransientBufferAllocator* RenderScene::GetDynamicVertexAllocator()
	{
		return m_dynamic_vertex_allocator.get();
	}

	TransientBufferAllocator* RenderScene::GetOverrideMaterialAllocator()
	{
		return m_override_material_allocator.get();
	}

	TransientBufferAllocator* RenderScene::GetBLASAllocator()
	{
		return m_result_blas_allocator.get();
	}

	TransientBufferAllocator* RenderScene::GetTLASAllocator()
	{
		return m_result_tlas_allocator.get();
	}

	TransientBufferAllocator* RenderScene::GetTLASScratchAllocator()
	{
		return m_scratch_tlas_allocator.get();
	}

	TransientBufferAllocator* RenderScene::GetTLASInstanceAllocator()
	{
		return m_instance_tlas_allocator.get();
	}

	TransientBufferAllocator* RenderScene::GetVertToInstanceAllocator()
	{
		return m_vert_to_inst_allocator.get();
	}

	TransientBufferAllocator* RenderScene::GetMatrixPaletteAllocator()
	{
		return m_matrix_palette_allocator.get();
	}

	TArray<AllocatorStats> RenderScene::GetAllocatorStats() const
	{
		TArray<AllocatorStats> stats;

		stats.push_back(CollectStats(m_instance_allocator.get()));
		stats.push_back(CollectStats(m_vert_to_inst_allocator.get()));

		stats.push_back(CollectStats(m_instance_tlas_allocator.get()));
		stats.push_back(CollectStats(m_scratch_tlas_allocator.get()));

		stats.push_back(CollectStats(m_result_tlas_allocator.get()));
		stats.push_back(CollectStats(m_result_blas_allocator.get()));

		stats.push_back(CollectStats(m_dynamic_vertex_allocator.get()));
		stats.push_back(CollectStats(m_override_material_allocator.get()));

		stats.push_back(CollectStats(m_light_allocator.get()));
		stats.push_back(CollectStats(m_volume_allocator.get()));
		stats.push_back(CollectStats(m_procedural_primitive_allocator.get()));

		stats.push_back(CollectStats(m_matrix_palette_allocator.get()));

		return stats;
	}

	RenderSceneStats RenderScene::GetStats() const
	{
		RenderSceneStats stats = {};
		stats.instance_count = m_instance_count;
		stats.light_count = m_light_count;
		

		return stats;
	}

}