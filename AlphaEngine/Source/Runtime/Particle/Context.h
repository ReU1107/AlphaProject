#pragma once
#include "Core/Types.h"

namespace Alpha
{
	class RenderCore;
	class CommandList;
	class DescriptorTable;
	class DescriptorHeap;
	class StructuredBuffer;
	class ByteAddressBuffer;
	class RingBuffer;
}

namespace Alpha::Particle
{
	// システム全体に共通で使用するもの
	struct Context
	{
		CommandList* command_list{ nullptr };
		DescriptorHeap* heap{ nullptr };
		
		RingBuffer* ring_buffer{ nullptr };
		
		DescriptorTable* table{ nullptr };
		StructuredBuffer* particle_pool{ nullptr };
		ByteAddressBuffer* dead_list{ nullptr };

		float delta_time{ 1.0f };

	};

	// 
	struct EmitContext
	{
		const Context& base;

		uint32_t emit_num{ 0 };
		uint32_t emit_max_num{ 0 };
		float4 color{};
		float life_time{ 1.0f };

	};

	struct UpdateContext
	{
		const Context& base;
		uint32_t particle_num{ 0 };
	};

}