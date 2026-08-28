#include "VolumetricPass.h"
#include "RenderCore/Module.h"
#include "Renderer/Module.h"

#include <imgui/imgui.h>

namespace Alpha
{
	VolumetricPass::~VolumetricPass()
	{
	}

	bool VolumetricPass::Initialize(const Args& args)
	{
		m_shader = RenderCore::Get().CreateShaderLibrary(L"Asset/Volumetric.hlsl",
			{ {
					L"RayGenerate",
					L"OpaqueHit",/* L"TranslucentHit", L"VolumeHit",
					L"OpaqueShadowHit" ,L"TranslucentShadowHit",L"VolumeShadowHit",
					L"VolumeIntersection",*/
					L"Miss"
				} }
		);

		RayTracePipelineArgs pipeline_args = {};
		pipeline_args.pipeline_layout = args.layout;

		pipeline_args.libraries.push_back(m_shader.get());
		pipeline_args.ray_gen_entry_name = L"RayGenerate";
		pipeline_args.miss_entry_names.push_back(L"Miss");

		{
			RHIRayTraceHitGroupDesc hit_group = {
				.name = L"Opaque",
				.type = ERHIHitGroupType::Triangles,
				.closest_hit_entry_name = L"OpaqueHit"
			};
			pipeline_args.hit_groups.push_back(hit_group);
		}

	
		pipeline_args.max_payload_size = 32;
		pipeline_args.max_attribute_size = 8;
		pipeline_args.max_recursion_depth = 1;

		m_pipeline = RenderCore::Get().CreateRayTracePipeline(pipeline_args);


		m_radiance_tex = RenderCore::Get().CreateTexture(480, 270, 1, ERHIResourceFormat::RGBAFloat, ETexturePresetType::RWTexture2D);
		m_transmittance_tex = RenderCore::Get().CreateTexture(480, 270, 1, ERHIResourceFormat::RGBAFloat, ETexturePresetType::RWTexture2D);

		m_radiance_tex->SetName(L"Radiance");
		m_transmittance_tex->SetName(L"Transmittance");

		m_froxel_radiance = args.froxel_radiance;
		m_depth = args.depth;

		return true;
	}

	void VolumetricPass::Finalize()
	{
	}

	struct PassConstants
	{
		UInt32 volume{};
		UInt32 depth{};
		UInt32 radiance{};
		UInt32 transmittance{};
	};

	void VolumetricPass::Execute(RenderContext& ctx)
	{
		ctx.command_list->SetRayTracePipeline(m_pipeline.get());

		PassConstants constants = {};
		constants.volume = m_froxel_radiance->GetSRVHandle().index;
		constants.depth = m_depth->GetSRVHandle().index;
		constants.radiance = m_radiance_tex->GetUAVHandle().index;
		constants.transmittance = m_transmittance_tex->GetUAVHandle().index;

		auto offset = RenderCore::Get().GetTransientAllocator()->WriteData<PassConstants>(&constants);

		ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

		Texture* textures[] = { m_radiance_tex.get(), m_transmittance_tex.get() };

		ctx.command_list->Transition(textures, 2, ERHIResourceState::UnorderedAccess);

		RayTraceDispatchArgs desc = {
			.pipeline = m_pipeline.get(),
			.width = 480,
			.height = 270,
			.depth = 1,
		};

		ctx.command_list->DispatchRayTrace(desc);


		ctx.command_list->UAVBarrier(textures, 2);
		ctx.command_list->Transition(textures, 2, ERHIResourceState::AllShaderResource);

		ImGui::Begin("VolumetricPass Pass");

		ImGui::Image(textures[0], ImVec2(960, 540));
		ImGui::Image(textures[1], ImVec2(960, 540));

		ImGui::End();
	}
}