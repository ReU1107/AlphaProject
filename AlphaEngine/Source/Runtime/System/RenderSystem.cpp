#include "RenderSystem.h"

#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include "Definitions.h"
#include "TransformComponent.h"

#include "Core/Math/MathF.h"

namespace Alpha
{
	using namespace Renderer;
	RenderSystem::~RenderSystem()
	{
	}
	static TUnique<Shader> as;
	static TUnique<Shader> ms;
	static TUnique<Shader> ps;
	static TUnique<Shader> cs;

	bool RenderSystem::Initialize(const Args& args)
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

		m_instances.reserve(10000);

		as = RenderCore::Get().CreateShader(L"Asset/TaskShader.hlsl", ERHIShaderType::Amplification);
		ms = RenderCore::Get().CreateShader(L"Asset/TestMeshShader.hlsl", ERHIShaderType::Mesh);
		ps = RenderCore::Get().CreateShader(L"Asset/PixelShader.hlsl", ERHIShaderType::Pixel);

		m_layout = RenderCore::Get().CreatePipelineLayout(
			[](PipelineLayout& layout)
			{
				layout.AddRootCBV(1, ERHIShaderFlags::All);
				layout.AddRootCBV(1, ERHIShaderFlags::All);
				layout.AddSampler(1, ERHIShaderFlags::Pixel);
				layout.SetFlags(ERHIPipelineLayoutFlags::Bindless);
			}
		);

		m_pipeline = RenderCore::Get().CreateGraphicsPipeline(
			{
				.pipeline_layout = m_layout.get(),
				.task_shader = as.get(),
				.mesh_shader = ms.get(),
				.pixel_shader = ps.get(),
				.render_target_count = 1,
				.render_targets = {
					RenderCore::Get().GetBackBufferFormat()
				},
				.blend_states = {
					DefaultAlphaBlend()
				},
				.depth_enable = true,

			}
		);

		cs = RenderCore::Get().CreateShader(L"Asset/AlignVertices.hlsl", ERHIShaderType::Compute);


		m_align_vertex_pipeline = RenderCore::Get().CreateComputePipeline(
			{
				.pipeline_layout = m_layout.get(),
				.compute_shader = cs.get(),
			}
		);

		return true;

	}

	void RenderSystem::Finalize()
	{

	}

	struct BufferIndices
	{
		RenderResourceManager::BufferIndices render_resourcce{};
		UInt32 instance{};
		UInt32 vertex_to_instance{};
		UInt32 dyanmic_position_srv{};      // 書き込まれた頂点
		UInt32 dyanmic_position_uav{};      // 書き込まれた頂点
	};

	struct Constants
	{
		float4x4 view_proj{ float4x4::Identity };
		float4 camera_pos;
		float4 frustum_planes;

		UInt32 instance_count;
	};

	static float3 camera_pos = Vector3(0.0f, 10.0f, -25.0f);

	void RenderSystem::Render(entt::registry& registry, RenderContext& ctx)
	{
		m_instance_allocator->Reset(ctx.current_frame_index);
		m_vert_to_inst_allocator->Reset(ctx.current_frame_index);
		m_dynamic_vertex_allocator->Reset(ctx.current_frame_index);
		m_result_tlas_allocator->Reset(ctx.current_frame_index);
		m_scratch_tlas_allocator->Reset(ctx.current_frame_index);
		m_result_blas_allocator->Reset(ctx.current_frame_index);

		m_instances.clear();

		BufferIndices indices = {
			.render_resourcce = RenderResourceManager::Get().GetBufferIndices(),
			.instance = m_instance_allocator->GetSRVIndex(),
			.vertex_to_instance = m_vert_to_inst_allocator->GetSRVIndex(),
			.dyanmic_position_srv = m_dynamic_vertex_allocator->GetSRVIndex(),
			.dyanmic_position_uav = m_dynamic_vertex_allocator->GetUAVIndex(),
		};

		auto buffers = RenderResourceManager::Get().GetBuffers();

		auto view = registry.view<MeshComponent, TransformComponent>();

		UInt64 vertex_count = 0;

		TLASRequest tlas_request = {};
		tlas_request.instance_desc_buffer = m_instance_tlas_allocator->GetBuffer();
		tlas_request.result_buffer = m_result_tlas_allocator->GetBuffer();

		// このフレームで描画する可能性のあるオブジェクトを全部詰め込む
		for (auto [ent, mesh_com, transform] : view.each())
		{
			uint32_t current_instance_id = static_cast<uint32_t>(m_instances.size());

			InstanceData instance = {};
			instance.world_matrix = transform.GetLocalToWorld();
			instance.mesh_offset = RenderResourceManager::Get().GetGPUMeshOffset(mesh_com.mesh_index);
			instance.material_table_offset = RenderResourceManager::Get().GetGPUMaterialTableOffset(mesh_com.material_table_index);
			
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

		m_instance_allocator->WriteData(m_instances.data(), m_instances.size());

		tlas_request.instance_count = m_instances.size();

		// ComputeShaderでのスキニングを行うにしろしないにしろこの頂点バッファに書き込む
		// 粒度は頂点単位
		ctx.command_list->SetComputePipeline(m_align_vertex_pipeline.get());

		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<BufferIndices>(&indices);

			ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 0);
		}

		ctx.command_list->Transition(m_dynamic_vertex_allocator->GetBuffer(), ERHIResourceState::UnorderedAccess);

		uint32_t dispatch_x = (vertex_count + 63) / 64;

		ctx.command_list->Dispatch(dispatch_x, 1, 1);


		ctx.command_list->UAVBarrier(m_dynamic_vertex_allocator->GetBuffer());

		ctx.command_list->Transition(m_dynamic_vertex_allocator->GetBuffer(), ERHIResourceState::NonPixelShaderResource);

		// ↑で作った頂点バッファを利用してBLASの更新
		RenderCore::Get().GetASBuilder()->RecordBuildCommands(ctx.command_list);
		ctx.command_list->UAVBarrier(m_result_blas_allocator->GetBuffer());

		// TLASの構築
		{
			RHIBuildTLASDesc desc = {};
			desc.instance_count = tlas_request.instance_count;
			desc.instance_desc_buffer = tlas_request.instance_desc_buffer->GetRHI();
			desc.result_as_buffer = m_result_tlas_allocator->GetBuffer()->GetRHI();
			desc.scratch_as_buffer = m_scratch_tlas_allocator->GetBuffer()->GetRHI();
			ctx.command_list->BuildRaytracingAccelerationStructure(desc);

		}
		
		ctx.command_list->UAVBarrier(m_result_tlas_allocator->GetBuffer());

		ctx.command_list->SetGraphicsPipeline(m_pipeline.get());

		if (GetKeyState('W') & 0x8000) { camera_pos.Z() += 0.2f; }
		if (GetKeyState('S') & 0x8000) { camera_pos.Z() -= 0.2f; }
		if (GetKeyState('A') & 0x8000) { camera_pos.X() -= 0.2f; }
		if (GetKeyState('D') & 0x8000) { camera_pos.X() += 0.2f; }

		// カメラコンポーネントから拾ってくる
		// 仮で
		Camera camera = {};
		camera.SetPosition(camera_pos);


		Constants constants = {};
		constants.view_proj = camera.GetViewProjectionMatrix();
		constants.instance_count = m_instances.size();

		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<BufferIndices>(&indices);

			ctx.command_list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 0);
		}

		{
			auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<Constants>(&constants);

			ctx.command_list->SetGraphicsCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 1);
		}

		ctx.command_list->SetGraphicsSamplerState(RenderCore::Get().GetPresetSampler(ESamplerPresetType::LinearClamp), 2);

		ctx.command_list->DispatchMesh(constants.instance_count, 1, 1);

	}
}