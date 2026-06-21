#include "GPUDataTransfer.h"
#include "RenderCore.h"

#include "CommandList.h"
#include "CommandQueue.h"
#include "Timeline.h"
#include "Texture.h"
#include "Buffer.h"

namespace
{
	uint64_t GetUploadAlignment(Alpha::UploadType type)
	{
		switch (type)
		{
		case Alpha::UploadType::Buffer	: return 4;
		case Alpha::UploadType::Texture	: return 512;
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


		m_staging_buffer = args.render_core->CreateBuffer(4, args.staging_buffer_size, EBufferPresetType::Upload);

		m_staging_allocator.Initialize(args.staging_buffer_size);

	}

	void GPUDataTransfer::AddBufferRegion(Buffer* buffer, UInt64 offset, void* data, UInt64 size)
	{
		UploadRequest request = {};
		request.type = UploadType::Buffer;
		request.resource = buffer;
		request.offset = offset;
		request.data = data;
		// buffer->GetSize();1データ＝バッファの一部になる可能性があるので、サイズも指定できるようにする
		request.size = size;
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

	UInt64 GPUDataTransfer::UploadAsync()
	{
		m_copy_list->Begin(0);

		for (auto& request : m_requests)
		{
			UInt64 offset = 0;
			UInt8* ptr = m_staging_buffer->GetMappedPtr();

			if (!m_staging_allocator.Allocate(request.size, GetUploadAlignment(request.type), &offset))
			{
				// バッファが一杯なのでこの段階で予定されてる分をアップロードする

			}
			UInt8* dest = ptr + offset;

			if (request.type == UploadType::Texture)
			{
				auto texture = static_cast<Texture*>(request.resource);
				texture->Upload(dest, request.data);

				m_copy_list->Transition(request.resource, ERHIResourceState::Common);

				m_copy_list->CopyTextureRegion(m_staging_buffer.get(), offset, texture);
			}
			else
			{
				auto buffer = static_cast<Buffer*>(request.resource);

				memcpy(dest, request.data, request.size);

				m_copy_list->Transition(request.resource, ERHIResourceState::Common);

				m_copy_list->CopyBufferRegion(m_staging_buffer.get(), offset, buffer, request.offset, request.size);
			}
		}

		m_copy_list->End();

		m_copy_queue->Execute(m_copy_list.get());
		auto fence_value = m_copy_queue->Signal(m_copy_fence.get());
		
		m_requests.clear();
		//m_staging_buffer->Reset();

		return fence_value;
	}

	void GPUDataTransfer::Upload()
	{
		auto fence_value = UploadAsync();

		m_copy_fence->WaitCPU();
	}
}