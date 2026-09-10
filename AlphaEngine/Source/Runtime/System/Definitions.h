#pragma once
#include "Core/Common.h"
#include "Renderer/Definitions.h"

namespace Alpha
{



	struct SystemTextureIndices
	{
		UInt32 result{};
		UInt32 visibility{};
		UInt32 normal{};
		UInt32 depth{};
	};

	struct FrameRenderContext
	{
		float4x4 view{ float4x4::Identity };
		float4x4 view_proj{ float4x4::Identity };
		float4x4 inv_view_proj{ float4x4::Identity };
		float4 camera_pos;
		float4 frustum_planes;
		float3 camera_up;
		float3 camera_right;

		UInt32 instance_count;
		UInt32 light_count;
	};

}