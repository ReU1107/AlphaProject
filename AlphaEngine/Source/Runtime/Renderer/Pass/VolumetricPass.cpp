#include "VolumetricPass.h"
#include "RenderCore/Module.h"
#include "../RenderGraph.h"
#include "../RenderGraphBuilder.h"

namespace Alpha::Renderer
{

	struct VolumetricPassConstants
	{
		UInt32 volume{};
		UInt32 depth{};
		UInt32 radiance{};
		UInt32 transmittance{};
	};

	void AddVolumetricPass(RenderCore* render_core, PipelineLayout* layout, RenderGraph* graph)
	{
		TArray<String> entries = { L"RayGenerate", L"OpaqueHit", L"Miss" };
		Shader* shader_library = render_core->CreateShaderLibrary(L"Asset/VolumetricRT.hlsl",
			{ {
					L"RayGenerate",
					L"OpaqueHit",/* L"TranslucentHit", L"VolumeHit",
					L"OpaqueShadowHit" ,L"TranslucentShadowHit",L"VolumeShadowHit",
					L"VolumeIntersection",*/
					L"Miss"
			} }
		);

		RayTracePipelineArgs pipeline_args = {};
		pipeline_args.pipeline_layout = layout;
		pipeline_args.libraries.push_back(shader_library);
		pipeline_args.ray_gen_entry_name = L"RayGenerate";
		pipeline_args.miss_entry_names.push_back(L"Miss");

		RHIRayTraceHitGroupDesc hit_group = {
			.name = L"Opaque",
			.type = ERHIHitGroupType::Triangles,
			.closest_hit_entry_name = L"OpaqueHit"
		};
		pipeline_args.hit_groups.push_back(hit_group);

		pipeline_args.max_payload_size = 32;
		pipeline_args.max_attribute_size = 8;
		pipeline_args.max_recursion_depth = 1;

		RayTracePipeline* rtpso = render_core->CreateRayTracePipeline(pipeline_args);

		auto froxel_radiance = graph->CreateTexture(L"FroxelRadiance",
			{
				160, 90, 64,
				ERHIResourceDimension::Tex3D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);

		auto depth = graph->CreateTexture(L"Depth",
			{
				1920, 1080, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::DepthStencil,
				ERHIResourceFormat::Depth32
			}
		);

		auto radiance = graph->CreateTexture(L"RadianceVolume",
			{
				480, 270, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);

		auto transmittance = graph->CreateTexture(L"Transmittance",
			{
				480, 270, 1,
				ERHIResourceDimension::Tex2D,
				ERHIResourceBindFlags::ShaderResource | ERHIResourceBindFlags::UnorderedAccess,
				ERHIResourceFormat::RGBAFloat
			}
		);

		graph->AddPass(L"VolumetricLighting",
			[=](RenderGraphBuilder& builder)
			{
				builder.ReadTexture(froxel_radiance, ERHIResourceState::AllShaderResource);
				builder.ReadTexture(depth, ERHIResourceState::AllShaderResource);
				builder.WriteTexture(radiance, ERHIResourceState::UnorderedAccess);
				builder.WriteTexture(transmittance, ERHIResourceState::UnorderedAccess);
				builder.UsePipeline(rtpso, ERHIPipelineType::RayTracing);
			},
			[=](RenderGraphContext& ctx)
			{
				Texture* froxel_radiance_tex = ctx.graph->GetTexture(froxel_radiance);
				Texture* depth_tex = ctx.graph->GetTexture(depth);
				Texture* radiance_tex = ctx.graph->GetTexture(radiance);
				Texture* transmittance_tex = ctx.graph->GetTexture(transmittance);

				VolumetricPassConstants constants = {};
				constants.volume = froxel_radiance_tex->GetSRVHandle().index;
				constants.depth = depth_tex->GetSRVHandle().index;
				constants.radiance = radiance_tex->GetUAVHandle().index;
				constants.transmittance = transmittance_tex->GetUAVHandle().index;

				auto offset = RenderCore::Get().GetTransientAllocator()->WriteData(&constants);
				ctx.command_list->SetComputeCBV(RenderCore::Get().GetTransientAllocator()->GetBuffer(), offset, 2);

				RayTraceDispatchArgs desc = {
					.pipeline = rtpso,
					.width = 480,
					.height = 270,
					.depth = 1,
				};

				ctx.command_list->DispatchRayTrace(desc);
			}
		);
	}

	// バンディング直さないと
}