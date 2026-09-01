#include "SceneObject.h"
#include "Scene.h"

namespace Alpha
{
	SceneObject::SceneObject(Scene* scene, entt::entity entity, UInt64 uuid, StringView name)
		: m_scene(scene)
		, m_entity(entity)
		, m_uuid(uuid)
		, m_name(name)
	{
	}

	void SceneObject::AddChild(SceneObject* child)
	{
		m_scene->Reparent(child, this);
	}

	void SceneObject::Unparent()
	{
		m_scene->Reparent(this, nullptr);
	}

	entt::registry& SceneObject::GetRegistry()
	{
		return m_scene->GetRegistry();
	}

}