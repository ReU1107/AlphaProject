#include "ASBuilder.h"
#include "RenderCore.h"
#include "TransientBufferAllocator.h"
#include "CommandList.h"
#include "CommandQueue.h"
#include "Timeline.h"
#include "Buffer.h"

#include "Util.h"

namespace Alpha
{
	ASBuilder::~ASBuilder()
	{
	}

	void ASBuilder::Initialize(const Args& args)
	{
		m_scratch_blas_allocator = CreateTransientAllocator(args.render_core, 256, 64_MB, EBufferPresetType::RWByteAddress, L"ScratchASBuffer");
	}

	void ASBuilder::AddBuildRequest(const BLASRequest& request)
	{
		m_build_requests.push_back(request);
	}

	void ASBuilder::AddUpdateRequest(const BLASRequest& request)
	{
		m_update_requests.push_back(request);
	}

	void ASBuilder::RecordBuildCommands(CommandList* command_list)
	{
		RecordCommands(command_list, m_build_requests);
	}

	void ASBuilder::RecordUpdateCommands(CommandList* command_list)
	{
		RecordCommands(command_list, m_update_requests);
	}

	void ASBuilder::Reset(UInt32 current_buffer_index)
	{
		m_scratch_blas_allocator->Reset(current_buffer_index);
	}

	void ASBuilder::RecordCommands(CommandList* command_list, TArray<BLASRequest>& requests)
	{
		for (auto& request : requests)
		{
			RHIBuildBLASDesc build_desc = {};

			if (request.vertex_buffer)
			{
				build_desc.vertex_buffer = request.vertex_buffer->GetRHI();
				build_desc.vertex_offset = request.vertex_offset;
			}

			if (request.index_buffer)
			{
				build_desc.index_buffer = request.index_buffer->GetRHI();
				build_desc.index_offset = request.index_offset;
			}

			if (request.procedural_buffer)
			{
				build_desc.procedural_buffer = request.procedural_buffer->GetRHI();
				build_desc.procedural_offset = request.procedural_offset;
			}

			build_desc.result_as_buffer = request.result_buffer->GetRHI();
			build_desc.result_as_offset = request.result_offset;

			if (request.source_as_buffer)
			{
				build_desc.source_as_buffer = request.source_as_buffer->GetRHI();
				build_desc.source_as_offset = request.source_as_offset;
			}

			build_desc.scratch_as_buffer = m_scratch_blas_allocator->GetBuffer()->GetRHI();

			UInt64 scratch_offset = m_scratch_blas_allocator->AllocateOffset(request.scratch_size);

			build_desc.scratch_as_offset = scratch_offset;

			build_desc.vertex_count = request.vertex_count;

			for (auto& geo : request.geometries)
			{
				RHIBottomLevelGeometryDesc geometry =
				{
					.index_byte_offset = geo.index_byte_offset,
					.index_count = geo.index_count,
					.aabb_count = geo.procedural_count,
					.is_opaque = geo.is_opaque,
				};

				build_desc.geometries.push_back(geometry);
			}

			build_desc.allow_update = request.allow_update;

			command_list->BuildRaytracingAccelerationStructure(build_desc);
		}

		requests.clear();
	}
}