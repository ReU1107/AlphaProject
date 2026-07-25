#pragma once
#include "Core/Types.h"
#include "RHI/Types.h"
#include "RHI/Definitions.h"

namespace Alpha
{
	enum class EVertexElementType
	{
		Float1,
		Float2,
		Float3,
		Float4,
		UInt1,
		UInt2,
		UInt3,
		UInt4,
		Color,	// r8g8b8a8unorm
	};

	class VertexLayout
	{
	private:
		std::vector<RHIVertexElementDesc> m_elements{};
		UInt32 m_stride{ 0 };
	public:
		VertexLayout() = default;
		~VertexLayout() = default;

		VertexLayout& AddElement(StringView name, EVertexElementType type, UInt32 semantic_index = 0);
		
		const std::vector<RHIVertexElementDesc>& GetElements() const { return m_elements; }
		
		UInt32 GetStride() const { return m_stride; }

		RHIVertexLayoutDesc ToRHIDesc() const;

	};
}