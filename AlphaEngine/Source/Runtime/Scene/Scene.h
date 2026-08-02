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

	public:
		Scene();
		~Scene();
		void Initialize();
		void Update(float delta_time);

		entt::registry& GetRegistry() { return m_registry; }
	};
}