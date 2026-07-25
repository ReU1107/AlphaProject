#pragma once
#include "Core/Common.h"
#include "Definitions.h"

namespace Alpha
{
	class GraphicsPipeline
	{
	private:
		TUnique<RHIGraphicsPipeline> m_rhi{ nullptr };
		PipelineLayout* m_layout{ nullptr };	// 所有権は持たない
		
		ERHIPrimitiveTopology m_primitive_topology{ ERHIPrimitiveTopology::TriangleList };
	public:
		GraphicsPipeline();
		~GraphicsPipeline();


		void Initialize(const GraphicsPipelineArgs& args);

		void Finalize();

		RHIGraphicsPipeline* GetRHI();

		PipelineLayout* GetPipelineLayout();

		ERHIPrimitiveTopology GetPrimitiveTopology() const { return m_primitive_topology; }
	};

}