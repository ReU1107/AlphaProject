#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;
	class CommandList;

	class TransientBufferAllocator;

	class Texture;
	
	struct RenderContext
	{
		CommandList* command_list{ nullptr };
		TransientBufferAllocator* transient_buffer{ nullptr };
		UInt32 current_frame_index{ 0 };

		UInt64 current_camera_offset{ 0 };

		UInt32 instance_count{ 0 };
		Matrix4x4 inv_view_proj{ Matrix4x4::Identity };

		UInt32 width{ 0 };
		UInt32 height{ 0 };


		Texture* test = nullptr;	// 絶対に後で消すから
	};

}