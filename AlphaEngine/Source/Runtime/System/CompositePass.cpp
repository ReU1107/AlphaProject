#include "CompositePass.h"
#include "RenderCore/Module.h"
#include "Renderer/Module.h"


namespace Alpha
{
	CompositePass::~CompositePass() = default;
	bool CompositePass::Initialize(const Args& args)
	{
		m_shader = RenderCore::Get().CreateShader(L"Asset/CompositeCS.hlsl", ERHIShaderType::Compute);

		ComputePipelineArgs pipeline_args = {};
		pipeline_args.pipeline_layout = args.layout;
		pipeline_args.compute_shader = m_shader.get();

		m_pipeline = RenderCore::Get().CreateComputePipeline(pipeline_args);


		m_result = args.result;
		m_volumetric = args.volumetric;
		m_transmittance = args.transmittance;
		m_base = args.base;

		return true;

	}
	
	void CompositePass::Finalize()
	{
	}
	

	struct PassConstants
	{
		UInt32 result_index;
		UInt32 volumetric;
		UInt32 transmittance;
		UInt32 scene_base;
	};

	void CompositePass::Execute(RenderContext& ctx)
	{

		ctx.command_list->SetComputePipeline(m_pipeline.get());

		PassConstants constants = {};
		constants.result_index = m_result->GetUAVHandle().index;
		constants.volumetric = m_volumetric->GetSRVHandle().index;
		constants.transmittance = m_transmittance->GetSRVHandle().index;
		constants.scene_base = m_base->GetSRVHandle().index;

		auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<PassConstants>(&constants);

		ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

		ctx.command_list->Transition(m_result, ERHIResourceState::UnorderedAccess);

		UInt32 group_x = RoundUp(1920, 8);
		UInt32 group_y = RoundUp(1080, 8);
		ctx.command_list->Dispatch(group_x, group_y, 1);


		ctx.command_list->UAVBarrier(m_result);
		ctx.command_list->Transition(m_result, ERHIResourceState::AllShaderResource);

	}
}