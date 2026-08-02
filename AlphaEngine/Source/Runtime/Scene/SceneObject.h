#pragma once
#include "Core/Common.h"
#include <entt/entt.hpp>

namespace Alpha
{
	class SceneObject
	{
	private:
		SceneObject* m_parent{ nullptr };
		TArray<SceneObject*> m_children{};
		entt::entity m_entity{ entt::null };

		entt::registry* m_registry{ nullptr };

	public:
		SceneObject();

		void SetParent(SceneObject* parent);


	};

	struct HierarchyComponent 
	{
		entt::entity parent{ entt::null };
		UInt32 depth{ 0 };
	};

}