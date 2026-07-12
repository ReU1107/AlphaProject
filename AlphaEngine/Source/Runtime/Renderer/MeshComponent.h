#pragma once
#include "Core/Common.h"
#include "Core/Component.h"

namespace Alpha
{
	class ModelHandle;
	class MaterialHandle;

	class MeshComponent// : public Component
	{
	public:
		TShared<ModelHandle> m_handle{ nullptr };
		TSharedArray<MaterialHandle> m_material_table{};



	};
}