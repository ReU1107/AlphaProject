#pragma once
#include "Core/Types.h"

namespace Alpha::Renderer
{
	class Material;

	// メッシュを構成する部品(メッシュの分割)
	class SubMesh
	{
	private:
		TShared<Material> m_material{};	// 
		uint32_t m_index_offset{ 0 };	// 頂点インデックス開始位置
		uint32_t m_index_count{ 0 };	// 自身が使用する頂点インデックス数
	public:
		SubMesh();
		~SubMesh();

		void SetMaterial(TShared<Material> material) { m_material = material; }

		void SetIndexOffset(uint32_t location) { m_index_offset = location; }
		void SetIndexCount(uint32_t num) { m_index_count = num; }

		Material* GetMaterial() const { return m_material.get(); }

		uint32_t GetIndexOffset() const { return m_index_offset; }
		uint32_t GetIndexCount() const { return m_index_count; }
	};
}