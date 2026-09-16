#include "PipelineCache.h"
#include "PipelineLayout.h"
#include "GraphicsPipeline.h"
#include "ComputePipeline.h"
#include "RayTracePipeline.h"

namespace Alpha
{
	PipelineCache::~PipelineCache() = default;

	void PipelineCache::Initialize(const Args& args)
	{
		m_layout_cache.Initialize(100);
		m_rtpso_cache.Initialize(100);
		m_gpso_cache.Initialize(100);
		m_cpso_cache.Initialize(100);
		m_command_layout_cache.Initialize(100);
	}

}