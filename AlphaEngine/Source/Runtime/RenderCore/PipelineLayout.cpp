#include "PipelineLayout.h"
#include "RenderCore/RenderHardwareInterface.h"
#include "RHI/RHIPipelineLayout.h"

#include "Core/Math/HashUtil.h"

namespace Alpha
{
	PipelineLayout::PipelineLayout() = default;
	PipelineLayout::~PipelineLayout() = default;

	RHIPipelineLayout* PipelineLayout::GetRHI()
	{
		return m_rhi.get();
	}

	void PipelineLayout::AddCostans(UInt32 count, ERHIShaderFlags visible_shader)
	{
		RHIPipelineLayoutDesc::Param parameter = {};
		parameter.parameter_type = ERHIParameterType::RootConstants;
		parameter.range_num = count;
		parameter.register_index = m_cbv_count++;
		parameter.shader_flags = visible_shader;
		m_descriptor_tables.parameters.push_back(parameter);
	}

	void PipelineLayout::AddDescriptors(ERHIResourceViewType type, UInt32 slot_index, ERHIShaderFlags visible_shader)
	{
		RHIPipelineLayoutDesc::Param parameter = {};
		parameter.parameter_type = ERHIParameterType::RootDescriptors;
		parameter.view_type = type;
		parameter.register_index = slot_index;
		parameter.shader_flags = visible_shader;
		m_descriptor_tables.parameters.push_back(parameter);
	}

	void PipelineLayout::AddRootSRV(UInt32 num, ERHIShaderFlags visible_shader)
	{
		for (UInt32 i = 0; i < num; ++i)
		{
			AddDescriptors(ERHIResourceViewType::ShaderResourceView, m_srv_count, visible_shader);
			m_srv_count++;
		}
	}

	void PipelineLayout::AddRootCBV(UInt32 num, ERHIShaderFlags visible_shader)
	{
		for (UInt32 i = 0; i < num; ++i)
		{
			AddDescriptors(ERHIResourceViewType::ConstantBufferView, m_cbv_count, visible_shader);
			m_cbv_count++;
		}
	}

	void PipelineLayout::AddRootUAV(UInt32 num, ERHIShaderFlags visible_shader)
	{
		for (UInt32 i = 0; i < num; ++i)
		{
			AddDescriptors(ERHIResourceViewType::UnorderedAccessView, m_uav_count, visible_shader);
			m_uav_count++;
		}
	}

	void PipelineLayout::BeginDescriptorTable()
	{
		RHIPipelineLayoutDesc::Param parameter = {};
		parameter.parameter_type = ERHIParameterType::DescriptorTable;

		m_descriptor_tables.parameters.push_back(parameter);
	}

	void PipelineLayout::AddDescriptorTable(ERHIResourceViewType type, UInt32 slot_index, UInt32 count, ERHIShaderFlags visible_shader)
	{
		auto& parameter = m_descriptor_tables.parameters[m_descriptor_tables.parameters.size() - 1];
		auto& range = parameter.ranges[parameter.range_num++];

		range.resource_num = count;
		range.view_type = type;
		range.register_index = slot_index;
	}

	void PipelineLayout::AddDescriptorSRV(UInt32 array_count, ERHIShaderFlags visible_shader)
	{
		AddDescriptorTable(ERHIResourceViewType::ShaderResourceView, m_srv_count, array_count, visible_shader);
		m_srv_count += array_count;
	}

	void PipelineLayout::AddDescriptorCBV(UInt32 array_count, ERHIShaderFlags visible_shader)
	{
		AddDescriptorTable(ERHIResourceViewType::ConstantBufferView, m_cbv_count, array_count, visible_shader);
		m_cbv_count += array_count;
	}

	void PipelineLayout::AddDescriptorUAV(UInt32 array_count, ERHIShaderFlags visible_shader)
	{
		AddDescriptorTable(ERHIResourceViewType::UnorderedAccessView, m_uav_count, array_count, visible_shader);
		m_uav_count += array_count;
	}

	void PipelineLayout::EndDescriptorTable()
	{

	}

	void PipelineLayout::AddSampler(UInt32 num, ERHIShaderFlags visible_shader)
	{
		BeginDescriptorTable();

		AddDescriptorTable(ERHIResourceViewType::SamplerView, m_sam_count, num, visible_shader);
		m_sam_count += num;

		EndDescriptorTable();
	}

	UInt32 PipelineLayout::GetHash() const
	{
		UInt64 handle_id = 0;

		HashCombine(handle_id, m_cbv_count);
		HashCombine(handle_id, m_srv_count);
		HashCombine(handle_id, m_uav_count);
		HashCombine(handle_id, m_sam_count);

		const auto& desc = m_descriptor_tables;

		HashCombine(handle_id, static_cast<UInt32>(desc.flags));

		HashCombine(handle_id, static_cast<UInt64>(desc.parameters.size()));

		for (int i = 0; i < desc.parameters.size(); ++i)
		{
			const auto& param = desc.parameters[i];

			HashCombine(handle_id, param.register_index);
			HashCombine(handle_id, static_cast<UInt32>(param.view_type));
			HashCombine(handle_id, static_cast<UInt32>(param.shader_flags));
			HashCombine(handle_id, static_cast<UInt32>(param.parameter_type));
			HashCombine(handle_id, param.range_num);

			if (param.parameter_type == ERHIParameterType::DescriptorTable)
			{
				for (UInt32 j = 0; j < param.range_num; ++j)
				{
					const auto& range = param.ranges[j];
					HashCombine(handle_id, range.resource_num);
					HashCombine(handle_id, range.register_index);
					HashCombine(handle_id, static_cast<UInt32>(range.view_type));
				}
			}
		}

		return handle_id;
	}

	void PipelineLayout::Build(RenderHardwareInterface* rhi)
	{
		m_rhi = rhi->CreatePipelineLayout(m_descriptor_tables);
	}
}