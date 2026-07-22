#pragma once
#include "AssetHandle.h"
#include "Renderer/Definitions.h"

namespace Alpha
{
	class MaterialHandle;

	class MaterialTableHandle : public AssetHandle
	{
	private:
		TSharedArray<MaterialHandle> m_referenced_materials{};


	public:
		~MaterialTableHandle();

		static AssetType Type() { return AssetType::MaterialTable; }

		AssetType GetType() const final { return AssetType::MaterialTable; }

		void AddMaterial(TShared<MaterialHandle> material) { m_referenced_materials.push_back(material); }

		TSharedArray<MaterialHandle>& GetMaterialHandles() { return m_referenced_materials; }
		const TSharedArray<MaterialHandle>& GetMaterialHandles() const { return m_referenced_materials; }

		void Accept(IVisitor* visitor) override {}

	};

}