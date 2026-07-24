#include "RenderSystem.h"
#include "RenderCore/Module.h"
#include "Renderer/RenderContext.h"

namespace Alpha
{
	using namespace Renderer;
	RenderSystem::~RenderSystem()
	{
	}

	bool RenderSystem::Initialize(const Args& args)
	{
		m_instance_allocator = CreateTransientAllocator(args.render_core, sizeof(Renderer::InstanceData), 4_MB, EBufferPresetType::UploadStructured, L"Instance");
		
		m_result_as_allocator = CreatePersistentAllocator(args.render_core, sizeof(Renderer::InstanceData), 64_MB, EBufferPresetType::UploadStructured, L"ASResult");
		
		m_scratch_as_allocator = CreateTransientAllocator(args.render_core, sizeof(Renderer::InstanceData), 4_MB, EBufferPresetType::UploadStructured, L"ASScratch");
		m_update_as_allocator = CreateTransientAllocator(args.render_core, sizeof(Renderer::InstanceData), 4_MB, EBufferPresetType::UploadStructured, L"ASUpdate");
		m_dynamic_vertex_allocator = CreateTransientAllocator(args.render_core, sizeof(Vector3), 4_MB, EBufferPresetType::RWStructured, L"DynamicVertex");



		return true;

	}

	void RenderSystem::Finalize()
	{
	}


	void RenderSystem::Render(RenderContext& ctx)
	{
		m_instance_allocator->Reset(ctx.current_frame_index);

		m_instances.clear();

		// このフレームで描画する可能性のあるオブジェクトを全部詰め込む

		m_instance_allocator->WriteData(m_instances.data(), m_instances.size());



	}
}