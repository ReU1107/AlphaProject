#pragma once
#include "Core/Types.h"

namespace Alpha
{
	class RenderCore;
	class CommandList;

	class TransientBufferAllocator;
	
	struct RenderContext
	{
		CommandList* command_list{ nullptr };
		TransientBufferAllocator* transient_buffer{ nullptr };
		UInt32 current_frame_index{ 0 };

		UInt64 current_camera_offset{ 0 };

		UInt32 width{ 0 };
		UInt32 height{ 0 };

	};

}