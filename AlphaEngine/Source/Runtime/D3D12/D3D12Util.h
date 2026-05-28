#pragma once

#if RHI_D3D12

#include <d3d12.h>
#include <dxgi1_6.h> 
#include <vector>
namespace Alpha
{
	template<D3D12_PIPELINE_STATE_SUBOBJECT_TYPE Type, typename T>
	struct PipelineStateSubobject
	{
		D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type = Type;
		T data{};
	};


	//https://microsoft.github.io/DirectX-Specs/d3d/DepthBoundsTest.html

	template<typename T>
	void AddSubobject(std::vector<uint8_t>& container, const T& data)
	{
		size_t current = container.size();
		size_t padding = (8 - (current % 8)) % 8;
		container.insert(container.end(), padding, 0);

		const uint8_t* p = reinterpret_cast<const uint8_t*>(&data);
		container.insert(container.end(), p, p + sizeof(T));
	}
}

#endif