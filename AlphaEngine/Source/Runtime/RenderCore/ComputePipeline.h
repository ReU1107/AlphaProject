#pragma once
#include "Core/Common.h"
#include "Definitions.h"

namespace Alpha
{
	class ComputePipeline
	{
	private:
		TUnique<RHIComputePipeline> m_rhi{ nullptr };
		PipelineLayout* m_layout{ nullptr };

	public:
		ComputePipeline();
		~ComputePipeline();


		void Initialize(const ComputePipelineArgs& args);

		void Finalize() {};

		RHIComputePipeline* GetRHI() const;

		PipelineLayout* GetPipelineLayout() const { return m_layout; }


	};

}