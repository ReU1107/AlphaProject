#include "RenderSystem.h"
#include "RenderCore/Module.h"

namespace Alpha
{
	using namespace Renderer;
	RenderSystem::~RenderSystem()
	{
	}

	bool RenderSystem::Initialize(const Args& args)
	{
		m_instance_allocator = std::make_unique<TransientBufferAllocator>();
		{
			TransientBufferAllocator::Args desc = {
				.render_core = args.render_core,
				.stride = sizeof(Renderer::InstanceData),
				.size = 4_MB,
				.type = EBufferPresetType::UploadStructured,
			};
			m_instance_allocator->Initialize(desc);
			m_instance_allocator->GetBuffer()->SetName(L"Instance");
		}
		return true;

	}

	void RenderSystem::Finalize()
	{
	}

	//void RenderSystem::Render(RenderContext& ctx)
	//{
	//	m_instance_allocator->Reset(ctx.current_frame_index);

	//	m_instances.clear();

	//	// このフレームで描画する可能性のあるオブジェクトを全部詰め込む

	//	m_instance_allocator->WriteData(m_instances.data(), m_instances.size());



	//}
}