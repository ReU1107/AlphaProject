#include "RenderScene.h"
#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include "TransformComponent.h"
#include "VertexComputePass.h"


#include "Core/Math/MathF.h"

namespace Alpha
{
	using namespace Renderer;

	RenderScene::~RenderScene() = default;

	bool RenderScene::Initialize(const Args& args)
	{
		m_instance_allocator = CreateTransientAllocator(args.render_core, sizeof(InstanceData), 4_MB, EBufferPresetType::UploadStructured, L"Instance");
		m_vert_to_inst_allocator = CreateTransientAllocator(args.render_core, sizeof(UInt32), 4_MB, EBufferPresetType::UploadStructured, L"VertexToInstance");

		// 暫定
		TLASRequest request = {
			.instance_count = 1000
		};

		ASBuildSize build_size = args.render_core->PreBuildTLAS(request);

		m_instance_tlas_allocator = CreateTransientAllocator(args.render_core, sizeof(ASInstanceData), 4_MB, EBufferPresetType::UploadStructured, L"TLASInstance");

		m_scratch_tlas_allocator = CreateTransientAllocator(args.render_core, 256, build_size.scratch, EBufferPresetType::RWByteAddress, L"TLASScratch");

		m_result_tlas_allocator = CreateTransientAllocator(args.render_core, 256, build_size.result, EBufferPresetType::RTAccelStructure, L"TLASResult");
		m_result_blas_allocator = CreateTransientAllocator(args.render_core, 256, 64_MB, EBufferPresetType::RTAccelStructure, L"BLASResult");

		m_dynamic_vertex_allocator = CreateTransientAllocator(args.render_core, sizeof(Vector3), 4_MB, EBufferPresetType::RWStructured, L"DynamicVertex");
		m_override_material_allocator = CreateTransientAllocator(args.render_core, sizeof(MaterialData), 4_MB, EBufferPresetType::UploadStructured, L"OverrideMaterial");
		
		m_light_allocator = CreateTransientAllocator(args.render_core, sizeof(LightData), 4_MB, EBufferPresetType::UploadStructured, L"Light");

		m_lights.reserve(100);
		m_instances.reserve(10000);


		m_vertex_compute_pass = std::make_unique<VertexComputePass>();
		m_vertex_compute_pass->Initialize({ args.render_core, args.layout });

		return true;
	}

	void RenderScene::Finalize()
	{
	}

	void RenderScene::Build(entt::registry& registry, RenderContext& ctx)
	{
		m_instance_allocator->Reset(ctx.current_frame_index);
		m_vert_to_inst_allocator->Reset(ctx.current_frame_index);
		m_dynamic_vertex_allocator->Reset(ctx.current_frame_index);
		m_result_tlas_allocator->Reset(ctx.current_frame_index);
		m_scratch_tlas_allocator->Reset(ctx.current_frame_index);
		m_result_blas_allocator->Reset(ctx.current_frame_index);
		m_override_material_allocator->Reset(ctx.current_frame_index);
		m_light_allocator->Reset(ctx.current_frame_index);

		m_instances.clear();

		SystemBufferIndices buffer_indices = {
			.static_res = RenderResourceManager::Get().GetBufferIndices(),
			.dynamic_res = GetBufferIndices(),
		};

		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<SystemBufferIndices>(&buffer_indices);

			ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 0);
			ctx.command_list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 0);
		}

		auto buffers = RenderResourceManager::Get().GetBuffers();

		auto view = registry.view<MeshComponent, TransformComponent>();

		UInt64 vertex_count = 0;

		TLASRequest tlas_request = {};
		tlas_request.instance_desc_buffer = m_instance_tlas_allocator->GetBuffer();
		tlas_request.result_buffer = m_result_tlas_allocator->GetBuffer();

		// このフレームで描画する可能性のあるオブジェクトを全部詰め込む
		for (auto [entity, mesh_com, transform] : view.each())
		{
			uint32_t current_instance_id = static_cast<uint32_t>(m_instances.size());

			InstanceData instance = {};
			instance.world_matrix = transform.GetLocalToWorld();
			instance.mesh_offset = RenderResourceManager::Get().GetGPUMeshOffset(mesh_com.mesh_index);

			if (auto* material_com = registry.try_get<MaterialOverrideComponent>(entity))
			{
				UInt64 material_byte_offset = m_override_material_allocator->WriteData(material_com->materials.data(), material_com->materials.size());
				instance.override_material_element_offset = material_byte_offset / sizeof(MaterialData);
			}

			// このメッシュの頂点データを書き込む場所を予約する
			const auto& mesh = RenderResourceManager::Get().GetMeshByIndex(mesh_com.mesh_index);

			UInt64 vertex_byte_offset = m_dynamic_vertex_allocator->AllocateOffset(mesh.vertex_position_alloc.size);
			instance.vertex_element_offset = vertex_byte_offset / sizeof(Vector3);

			m_vert_to_inst_allocator->FillData(current_instance_id, mesh.vertex_count);

			UInt64 as_byte_offset = m_result_blas_allocator->AllocateOffset(mesh.as.size);

			vertex_count += mesh.vertex_count;

			BLASRequest request = {
				.vertex_buffer = m_dynamic_vertex_allocator->GetBuffer(),
				.vertex_offset = vertex_byte_offset,
				.index_buffer = buffers.index,
				.index_offset = mesh.index_alloc.offset,
				.result_buffer = m_result_blas_allocator->GetBuffer(),
				.result_offset = as_byte_offset,
				.source_as_buffer = buffers.source_as,
				.source_as_offset = mesh.as.source_alloc.offset,
				.scratch_size = mesh.as.scratch_size,
				.vertex_count = mesh.vertex_count,
			};

			for (auto& sub : mesh.submeshes)
			{
				request.geometries.push_back(
					{
						.index_byte_offset = sub.index_element_offset * sizeof(UInt32),
						.index_count = sub.index_count,
					}
					);
			}

			RenderCore::Get().BuildBLAS(request);

			ASInstanceData as_instance = {};
			as_instance.instance_id = current_instance_id;
			as_instance.blas_gpu_address = m_result_blas_allocator->GetBuffer()->GetGPUAddress() + as_byte_offset;
			as_instance.transform = MatrixTranspose(instance.world_matrix);

			m_instance_tlas_allocator->WriteData(&as_instance);


			m_instances.push_back(instance);
		}

		{

			m_lights.clear();

			auto view = registry.view<LightComponent, TransformComponent>();

			// ライトの情報を
			for (auto [entity, light_com, transform] : view.each())
			{

				LightData light = {};
				const Matrix4x4& world_matrix = transform.GetLocalToWorld();
				light.position = world_matrix._4();

				light.radius = light_com.GetRadius();
				light.color = light_com.GetColor();
				light.intensity = light_com.GetIntensity();

				light.type = light_com.type;

				if (light_com.type == 2) // Directional Light
				{
					light.direction = light_com.direction;
				}
				else if (light_com.type == 1) // Spot Light
				{
					light.direction = transform.Forward();
				}
				else
				{
					light.direction = float3(0.0f, 0.0f, 0.0f);
				}

				light.inner_cone = light_com.GetInnerCone();
				light.outer_cone = light_com.GetOuterCone();

				m_lights.push_back(light);
			}

			m_light_allocator->WriteData(m_lights.data(), m_lights.size());

		}

		// インスタンスデータをバッファに書き込む
		m_instance_allocator->WriteData(m_instances.data(), m_instances.size());

		// 頂点の整列やスキニング
		m_vertex_compute_pass->Execute(ctx, m_dynamic_vertex_allocator->GetBuffer(), vertex_count);

		// ↑で作った頂点バッファを利用してBLASの更新
		RenderCore::Get().GetASBuilder()->RecordBuildCommands(ctx.command_list);
		ctx.command_list->UAVBarrier(m_result_blas_allocator->GetBuffer());

		// TLASの構築
		{
			tlas_request.instance_count = m_instances.size();

			RHIBuildTLASDesc desc = {};
			desc.instance_count = tlas_request.instance_count;
			desc.instance_desc_buffer = tlas_request.instance_desc_buffer->GetRHI();
			desc.result_as_buffer = m_result_tlas_allocator->GetBuffer()->GetRHI();
			desc.scratch_as_buffer = m_scratch_tlas_allocator->GetBuffer()->GetRHI();
			ctx.command_list->BuildRaytracingAccelerationStructure(desc);

		}

		ctx.command_list->UAVBarrier(m_result_tlas_allocator->GetBuffer());
	}

	DynamicBufferIndices RenderScene::GetBufferIndices() const
	{
		return DynamicBufferIndices{
			.instance = m_instance_allocator->GetSRVIndex(),
			.vertex_to_instance = m_vert_to_inst_allocator->GetSRVIndex(),
			.current_position_srv = m_dynamic_vertex_allocator->GetSRVIndex(),
			.current_position_uav = m_dynamic_vertex_allocator->GetUAVIndex(),
			.override_material = m_override_material_allocator->GetSRVIndex(),
			.light = m_light_allocator->GetSRVIndex(),
			.rt_structure = m_result_tlas_allocator->GetSRVIndex(),
		};
	}
}