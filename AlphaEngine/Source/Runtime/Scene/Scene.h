#pragma once
#include "Core/Common.h"
#include <entt/entt.hpp>

namespace Alpha
{
	class SceneObject;

	class Scene
	{
	private:
		entt::registry m_registry{};
		TUniqueArray<SceneObject> m_objects;
		TArray<SceneObject*> m_root_objects;

	public:
		Scene();
		~Scene();
		void Initialize();
		void Update(float delta_time);

		entt::registry& GetRegistry() { return m_registry; }

		SceneObject& CreateObject(const String& name);

		SceneObject* FindObject(StringView name);

		bool Reparent(SceneObject* object, SceneObject* new_parent);

		void DetachFromHierarchy(SceneObject* object);

		void CompactHierarchy();

	};
}