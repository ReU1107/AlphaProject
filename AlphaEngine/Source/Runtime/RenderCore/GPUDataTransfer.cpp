#include "GPUDataTransfer.h"
#include "RenderCore.h"
#include "Resources.h"

#include "CommandList.h"
#include "CommandQueue.h"
#include "Timeline.h"
#include "Buffer/RingBuffer.h"

namespace
{
	uint64_t GetUploadAlignment(Alpha::UploadType type)
	{
		switch (type)
		{
		case Alpha::UploadType::Structured	: return 4;
		case Alpha::UploadType::Texture		: return 512;
		default:
			break;
		}
		return 4;
	}

}

namespace Alpha
{
	GPUDataTransfer::GPUDataTransfer() = default;
	GPUDataTransfer::~GPUDataTransfer() = default;

	void GPUDataTransfer::Initialize(const Args& args)
	{
		m_copy_queue = args.render_core->CreateCommandQueue(ERHICommandType::Copy);

		m_copy_list = args.render_core->CreateCommandList(ERHICommandType::Copy);

		m_copy_fence = args.render_core->CreateTimeline();

		m_staging_buffer = args.render_core->CreateRingBuffer(args.staging_buffer_size);

	}

	void GPUDataTransfer::AddStructuredBuffer(StructuredBuffer* buffer, void* data)
	{
		UploadRequest request = {};
		request.type = UploadType::Structured;
		request.resource = buffer;
		request.data = data;
		request.size = buffer->GetSize();
		m_requests.push_back(request);
	}

	void GPUDataTransfer::AddTexture(Texture* texture, void* data)
	{
		UploadRequest request = {};
		request.type = UploadType::Texture;
		request.resource = texture;
		request.data = data;
		request.size = texture->GetSize();
		m_requests.push_back(request);
	}

	uint64_t GPUDataTransfer::UploadAsync()
	{
		m_copy_list->Begin(0);

		for (auto& request : m_requests)
		{

			uint64_t offset = 0;
			void* ptr = m_staging_buffer->Allocate(request.size, GetUploadAlignment(request.type), &offset);

			if (request.type == UploadType::Texture)
			{
				auto texture = static_cast<Texture*>(request.resource);
				texture->Upload(ptr, request.data);

				m_copy_list->Transition(request.resource, ERHIResourceState::Common);

				m_copy_list->CopyTextureRegion(m_staging_buffer.get(), offset, texture);
			}
			else
			{
				auto buffer = static_cast<Buffer*>(request.resource);
				buffer->Upload(ptr, request.data, request.size);

				m_copy_list->Transition(request.resource, ERHIResourceState::Common);

				m_copy_list->CopyBufferRegion(m_staging_buffer.get(), offset, buffer);
			}
		}

		m_copy_list->End();

		m_copy_queue->Execute(m_copy_list.get());
		auto fence_value = m_copy_queue->Signal(m_copy_fence.get());
		m_requests.clear();
		return fence_value;
	}

	void GPUDataTransfer::Upload()
	{
		auto fence_value = UploadAsync();

		m_copy_fence->WaitCPU();
	}
}