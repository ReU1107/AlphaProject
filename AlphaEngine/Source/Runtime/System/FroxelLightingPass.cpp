#include "FroxelLightingPass.h"
#include "RenderCore/Module.h"
#include "Renderer/Module.h"
#include "Core/Math/MathUtil.h"

namespace Alpha
{
	static UInt32 ResolutionX = 160;
	static UInt32 ResolutionY = 90;
	static UInt32 ResolutionZ = 64;

	FroxelLightingPass::~FroxelLightingPass() = default;

	bool FroxelLightingPass::Initialize(const Args& args)
	{
		m_shader = RenderCore::Get().CreateShader(L"Asset/FroxelLightingCS.hlsl", ERHIShaderType::Compute);

		ComputePipelineArgs pipeline_args = {};
		pipeline_args.pipeline_layout = args.layout;
		pipeline_args.compute_shader = m_shader.get();

		m_pipeline = RenderCore::Get().CreateComputePipeline(pipeline_args);

		m_result = RenderCore::Get().CreateTexture(ResolutionX, ResolutionY, ResolutionZ, ERHIResourceFormat::RGBAFloat, ETexturePresetType::RWTexture3D);
		m_result->SetName(L"FroxelRadiance");

		return true;
	}

	void FroxelLightingPass::Finalize()
	{
	}

	struct PassConstants
	{
		UInt32 result_index;
	};

	void FroxelLightingPass::Execute(RenderContext& ctx)
	{
		ctx.command_list->SetComputePipeline(m_pipeline.get());

		PassConstants constants = {};
		constants.result_index = m_result->GetUAVHandle().index;

		auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<PassConstants>(&constants);

		ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

		UInt32 group_x = RoundUp(ResolutionX, 8);
		UInt32 group_y = RoundUp(ResolutionY, 8);
		UInt32 group_z = RoundUp(ResolutionZ, 4);
		ctx.command_list->Dispatch(group_x, group_y, group_z);

	}
}