#pragma once

namespace Alpha
{
	class RenderGraphBuilder;
	class RenderContext;

	class RenderPass
	{
	public:
		virtual ~RenderPass() {};

		virtual void Setup(RenderGraphBuilder& builder) = 0;

		virtual void Execute(RenderContext& ctx) = 0;

	};

}