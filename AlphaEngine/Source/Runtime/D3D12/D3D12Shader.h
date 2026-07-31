#pragma once
#include "D3D12Device.h"
#include "RHI/RHIShader.h"
#if RHI_D3D12

#include <dxcapi.h>

namespace Alpha
{
	class D3D12Shader : public RHIShader, public D3D12DeviceChild
	{
	private:
		ComPtr<IDxcBlob> m_shader_blob{ nullptr };

		std::wstring m_entry_point{ L"main" };

	public:
		D3D12Shader(D3D12Device* device);
		~D3D12Shader();

		bool Initialize(const RHIShaderDesc& desc) final;

		IDxcBlob* GetNativeHandle() const { return m_shader_blob.Get(); }

		const void* GetBytecodePointer() const { return m_shader_blob->GetBufferPointer(); }

		SIZE_T GetBytecodeLength() const { return m_shader_blob->GetBufferSize(); }

		const wchar_t* GetEntryPoint() const final { return m_entry_point.c_str(); }


	};
}

#endif