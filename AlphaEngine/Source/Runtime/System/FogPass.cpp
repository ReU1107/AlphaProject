#include "FogPass.h"
#include "RenderCore/Module.h"
#include "Renderer/Module.h"

namespace Alpha
{
	FogPass::~FogPass()
	{
	}

	bool FogPass::Initialize(const Args& args)
	{
		m_shader = RenderCore::Get().CreateShaderLibrary(L"Asset/Fog.hlsl", { { L"RayGenerate", L"TranslucentCheck", L"TranslucentHit", L"OpaqueHit", L"Miss" } });

		RayTracePipelineArgs pipeline_args = {};
		pipeline_args.pipeline_layout = args.layout;

		pipeline_args.libraries.push_back(m_shader.get());

		pipeline_args.ray_gen_entry_name = L"RayGenerate";
		pipeline_args.miss_entry_names.push_back(L"Miss");

		RHIRayTraceHitGroupDesc hit_group = {
			.name = L"Opacity",
			.type = ERHIHitGroupType::Triangles,
			.closest_hit_entry_name = L"OpaqueHit"
		};
		pipeline_args.hit_groups.push_back(hit_group);

		pipeline_args.max_payload_size = 32;
		pipeline_args.max_attribute_size = 8;
		pipeline_args.max_recursion_depth = 1;

		m_pipeline = RenderCore::Get().CreateRayTracePipeline(pipeline_args);


		//m_result = RenderCore::Get().CreateTexture(ResolutionX, ResolutionY, ResolutionZ, ERHIResourceFormat::RGBAFloat, ETexturePresetType::RWTexture3D);

		return false;
	}

	void FogPass::Finalize()
	{
	}

	struct PassConstants
	{
		UInt32 result_index;
	};


	void FogPass::Execute(RenderContext& ctx)
	{
		ctx.command_list->SetRayTracePipeline(m_pipeline.get());

		PassConstants constants = {};
		constants.result_index = m_result->GetUAVHandle().index;

		auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<PassConstants>(&constants);

		ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

	/*	RayTraceDispatchArgs desc = {
			.pipeline = m_pipeline.get(),
			.width = ResolutionX,
			.height = ResolutionY,
			.depth = ResolutionZ,
		};

		ctx.command_list->DispatchRayTrace(desc);*/

	}
}