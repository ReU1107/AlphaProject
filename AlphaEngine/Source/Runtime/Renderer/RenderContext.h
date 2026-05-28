#pragma once
#include "Core/Types.h"

namespace Alpha
{
	class RenderCore;
	class CommandList;

	class RingBuffer;

	struct RenderContext
	{
		CommandList* command_list{ nullptr };
		RingBuffer* transient_buffer{ nullptr };

		uint64_t current_camera_offset{ 0 };

		uint32_t width{ 0 };
		uint32_t height{ 0 };

	};

}