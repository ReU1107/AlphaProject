#include "RenderSystem.h"

#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include "Definitions.h"
#include "TransformComponent.h"

namespace Alpha
{
	using namespace Renderer;
	RenderSystem::~RenderSystem()
	{
	}
	static TUnique<Shader> as;
	static TUnique<Shader> ms;
	static TUnique<Shader> ps;

	bool RenderSystem::Initialize(const Args& args)
	{
		m_instance_allocator = CreateTransientAllocator(args.render_core, sizeof(InstanceData), 4_MB, EBufferPresetType::UploadStructured, L"Instance");

		m_result_as_allocator = CreatePersistentAllocator(args.render_core, 256, 64_MB, EBufferPresetType::RTAccelStructure, L"ASResult");

		// 一旦updateは使わんか?
		//m_update_as_allocator = CreateTransientAllocator(args.render_core, 256, 4_MB, EBufferPresetType::RWByteAddress, L"ASUpdate");

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

		return true;

	}

	void RenderSystem::Finalize()
	{
	}

	struct BufferIndices
	{
		RenderResourceManager::BufferIndices render_resourcce{};
		UInt32 instance{};
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
		RenderCore::Get().GetASBuilder()->RecordBuildCommands(ctx.command_list);

		return;



		m_instance_allocator->Reset(ctx.current_frame_index);
		m_dynamic_vertex_allocator->Reset(ctx.current_frame_index);

		m_instances.clear();

		BufferIndices indices = {
			.render_resourcce = RenderResourceManager::Get().GetBufferIndices(),
			.instance = m_instance_allocator->GetBuffer()->GetSRVHandle().index,
		};


		auto view = registry.view<MeshComponent, TransformComponent>();

		// このフレームで描画する可能性のあるオブジェクトを全部詰め込む
		for (auto [ent, mesh_com, transform] : view.each())
		{
			InstanceData instance = {};
			instance.world_matrix = transform.GetLocalToWorld();
			instance.mesh_offset = RenderResourceManager::Get().GetGPUMeshOffset(mesh_com.mesh_index);
			instance.material_table_offset = RenderResourceManager::Get().GetGPUMaterialTableOffset(mesh_com.material_table_index);
			
			// このメッシュの頂点データを書き込む場所を予約する
			const auto& mesh = RenderResourceManager::Get().GetMeshByIndex(mesh_com.mesh_index);
			
			UInt64 offset = 0;
			m_dynamic_vertex_allocator->Allocate(mesh.vertex_position_alloc.size, &offset);

			instance.vertex_offset = offset;
			// 一旦読み込んだままの情報で表示
			instance.vertex_offset = mesh.vertex_position_alloc.offset;

			m_instances.push_back(instance);

			//m_result_as_allocator->AllocateBySize(mesh.as.size, );


		}

		m_instance_allocator->WriteData(m_instances.data(), m_instances.size());

		/*
		* ComputeShaderでのスキニングを行うにしろしないにしろこの頂点バッファに書き込む
		*/
		m_dynamic_vertex_allocator->GetBuffer()->GetUAVHandle().index;



		ctx.command_list->UAVBarrier(m_result_as_allocator->GetBuffer());


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