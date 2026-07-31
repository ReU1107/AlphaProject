#pragma once
#include "D3D12Device.h"
#include "RHI/RHIRayTracePipeline.h"
#if RHI_D3D12

namespace Alpha
{
	class D3D12RayTracePipeline : public RHIRayTracePipeline, public D3D12DeviceChild
	{
	private:
		ComPtr<ID3D12StateObject> m_state_object{ nullptr };
		ComPtr<ID3D12StateObjectProperties1> m_state_object_props{ nullptr };


	public:
		D3D12RayTracePipeline(D3D12Device* device);
		~D3D12RayTracePipeline();

		bool Initialize(const RHIRayTracePipelineDesc& desc) final;

		void* GetShaderIdentifier(const StringView export_nmae) const final;

		ID3D12StateObject* GetNativeHandle() const { return m_state_object.Get(); }

	};

}

#endif
