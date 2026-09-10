#pragma once
#include "Core/Common.h"
#include "Definitions.h"

#include <functional>

namespace Alpha::Renderer
{

	using PassExecuteFunc = std::function<void(RenderGraphContext&)>;

	struct RenderGraphPass
	{
		String name{};
		TArray<RGTextureUsage> textures{};
		RGPipelineUsage pipeline{};


		PassExecuteFunc execute{};
	};

}