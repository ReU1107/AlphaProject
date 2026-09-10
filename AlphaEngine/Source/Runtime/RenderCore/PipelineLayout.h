#pragma once
#include "Core/Common.h"
#include "RHI/Types.h"
#include "RHI/Definitions.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class RHIPipelineLayout;
	
	class PipelineLayout
	{
	private:
		TUnique<RHIPipelineLayout> m_rhi{ nullptr };
		RHIPipelineLayoutDesc m_descriptor_tables{};
		UInt32 m_cbv_count{ 0 };
		UInt32 m_srv_count{ 0 };
		UInt32 m_uav_count{ 0 };
		UInt32 m_sam_count{ 0 };

	public:
		PipelineLayout();
		~PipelineLayout();

		RHIPipelineLayout* GetRHI();

		void AddCostans(UInt32 count, ERHIShaderFlags visible_shader = ERHIShaderFlags::All);

		void AddDescriptors(ERHIResourceViewType type, UInt32 slot_index, ERHIShaderFlags visible_shader);

		void AddRootSRV(UInt32 num, ERHIShaderFlags visible_shader = ERHIShaderFlags::All);
		void AddRootCBV(UInt32 num, ERHIShaderFlags visible_shader = ERHIShaderFlags::All);
		void AddRootUAV(UInt32 num, ERHIShaderFlags visible_shader = ERHIShaderFlags::All);

		void BeginDescriptorTable();
		
		void AddDescriptorTable(ERHIResourceViewType type, UInt32 slot_index, UInt32 count, ERHIShaderFlags visible_shader);
		
		void AddDescriptorSRV(UInt32 array_count, ERHIShaderFlags visible_shader = ERHIShaderFlags::All);
		void AddDescriptorCBV(UInt32 array_count, ERHIShaderFlags visible_shader = ERHIShaderFlags::All);
		void AddDescriptorUAV(UInt32 array_count, ERHIShaderFlags visible_shader = ERHIShaderFlags::All);
		
		void EndDescriptorTable();

		void AddSampler(UInt32 num, ERHIShaderFlags visible_shader = ERHIShaderFlags::All);

		void SetFlags(ERHIPipelineLayoutFlags flags) { m_descriptor_tables.flags = flags; }

		UInt32 GetHash() const;

		void Build(RenderHardwareInterface* rhi);
	};

}

/*
* Constants
* 頻繁に切り替わる軽量データ
* countはfloat(32bit)の数にする
* float4ならcountは4
* Descriptors
* AddRoot〇〇View() 
* 
* DescriptorTable
* 複数のテクスチャ等
* void BeginDescriptorTable()
* この間でAddDescriptor〇〇View();を呼び出してください
* void EndDescriptorTable()
*/

/*
* temp.hlsl
* 
* ConstantBuffer<A> a : register(b0);
* ConstantBuffer<B> b : register(b1);
* 
* 
* 
*/