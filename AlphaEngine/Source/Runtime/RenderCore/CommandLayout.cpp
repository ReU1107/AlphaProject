#include "CommandLayout.h"
#include "RHI/RHICommandLayout.h"
#include "RenderHardwareInterface.h"
#include "PipelineLayout.h"

namespace Alpha
{
	CommandLayout::CommandLayout() = default;
	CommandLayout::~CommandLayout() = default;

	void CommandLayout::Initialize(const CommandLayoutArgs& args)
	{
		RHICommandLayoutDesc desc = {};
		desc.layout = args.pipeline_layout ? args.pipeline_layout->GetRHI() : nullptr;
		desc.stride = 0;

		desc.arguments.reserve(args.arguments.size());

		for (const auto& arg : args.arguments)
		{
			RHICommandLayoutDesc::Argument argument = {
				.type = arg.type,
				.root_parameter_index = arg.root_parameter_index,
				.dest_runtime_constant_offset = arg.dest_runtime_constant_offset,
				.num_32bit_values_to_set = arg.num_32bit_values_to_set,
				.slot_index = arg.slot_index,
			};

			desc.arguments.push_back(argument);
		}

		m_rhi = args.rhi->CreateCommandLayout(desc);
	}

	RHICommandLayout* CommandLayout::GetRHI() const
	{
		return m_rhi.get();
	}
}