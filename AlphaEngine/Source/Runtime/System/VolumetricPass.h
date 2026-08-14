#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;
	class PipelineLayout;
	class Shader;
	class Texture;
	class RayTracePipeline;

	struct RenderContext;

	class VolumetricPass
	{
	private:
		TUnique<RayTracePipeline> m_pipeline{ nullptr };
		TUnique<Shader> m_shader{ nullptr };
		TUnique<Texture> m_radiance_tex{};
		TUnique<Texture> m_transmittance_tex{};
		Texture* m_froxel_radiance{ nullptr };
		Texture* m_depth{ nullptr };
	public:

		~VolumetricPass();
		struct Args
		{
			RenderCore* render_core{ nullptr };
			PipelineLayout* layout{ nullptr };
			Texture* froxel_radiance{ nullptr };
			Texture* depth{ nullptr };
		};
		bool Initialize(const Args& args);
		void Finalize();

		void Execute(RenderContext& ctx);

		Texture* GetRadianceTex() const { return m_radiance_tex.get(); }
		Texture* GetTransmittanceTex() const { return m_transmittance_tex.get(); }


	};
}