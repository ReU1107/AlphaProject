#include "ComputePipeline.h"
#include "PipelineLayout.h"
#include "RenderCore/RenderHardwareInterface.h"
#include "RenderCore/Shader.h"
#include "RHI/RHIComputePipeline.h"

namespace Alpha
{
	ComputePipeline::ComputePipeline() = default;
	ComputePipeline::~ComputePipeline() = default;

	void ComputePipeline::Initialize(const ComputePipelineArgs& args)
	{
		RHIComputePipelineDesc desc = {};
		desc.layout = args.pipeline_layout->GetRHI();
		desc.compute_shader = args.compute_shader->GetRHI();

		m_rhi = args.rhi->CreateComputePipeline(desc);

		m_layout = args.pipeline_layout;
	}

	RHIComputePipeline* ComputePipeline::GetRHI() const
	{
		return m_rhi.get();
	}
	
}