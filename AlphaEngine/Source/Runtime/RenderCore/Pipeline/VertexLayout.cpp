#include "VertexLayout.h"
#include "Core/Assertion.h"

namespace Alpha
{
	VertexLayout& VertexLayout::AddElement(StringView name, EVertexElementType type, UInt32 semantic_index)
	{
		RHIVertexElementDesc element = {};
		element.semantic_name = name;
		element.semantic_index = semantic_index;
		// タイプに応じてストライドを計算
		switch (type)
		{
		case EVertexElementType::Float1	: element.format = ERHIResourceFormat::RFloat;		break;
		case EVertexElementType::Float2	: element.format = ERHIResourceFormat::RGFloat;		break;
		case EVertexElementType::Float3	: element.format = ERHIResourceFormat::RGBFloat;	break;
		case EVertexElementType::Float4	: element.format = ERHIResourceFormat::RGBAFloat;	break;
		case EVertexElementType::UInt1	: element.format = ERHIResourceFormat::RGBA32UInt;	break;
		case EVertexElementType::UInt2	: element.format = ERHIResourceFormat::RGBA32UInt;	break;
		case EVertexElementType::UInt3	: element.format = ERHIResourceFormat::RGBA32UInt;	break;
		case EVertexElementType::UInt4	: element.format = ERHIResourceFormat::RGBA32UInt;	break;

		case EVertexElementType::Color	: element.format = ERHIResourceFormat::RGBAUnorm;	break;
		default:
			//Assert(!"VertexLayout::AddElement: 未知の頂点要素タイプです。");
			break;
		}
		m_elements.push_back(element);

		return *this;
	}

	RHIVertexLayoutDesc VertexLayout::ToRHIDesc() const
	{
		RHIVertexLayoutDesc desc = {};
		for (size_t i = 0; i < m_elements.size(); ++i)
		{
			desc.elements[i] = m_elements[i];
			desc.element_count++;
		}

		return desc;
	}

}