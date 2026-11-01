#pragma once
#include "Definitions.h"

namespace Alpha::Renderer
{

	struct ClusterLightResources
	{
		RGTextureHandle cluster_texture;
		RGBufferHandle cluster_buffer;
		RGBufferHandle light_indices;
	};

	struct FroxelLightingResources
	{
		RGTextureHandle froxel_radiance_texture;
	};

	struct VBufferResources
	{
		RGTextureHandle visibility_texture;
		RGTextureHandle depth_texture;
	};

	struct VolumetricResources
	{
		RGTextureHandle radiance_texture;
		RGTextureHandle transmittance_texture;
	};

}