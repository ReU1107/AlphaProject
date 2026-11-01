#pragma once
#include "Core/Common.h"

namespace Alpha::Renderer
{
	struct ClusterGridSettings
	{
		UVector3 grid_dimensions = { 32, 24, 16 };
		UInt32 max_lights_per_cluster{ 32 };

		float near_z{ 0.1f };
		float far_z{ 1000.0f };
	};

	struct FroxelLightingSettings
	{
		UVector3 grid_dimensions = { 160, 90, 64 };

		float near_z{ 0.1f };
		float far_z{ 1000.0f };
	};


}