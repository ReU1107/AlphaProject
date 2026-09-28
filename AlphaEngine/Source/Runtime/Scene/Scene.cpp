#include "Scene.h"
#include "SceneObject.h"
#include "Spatial/TransformComponent.h"

#include "Core/Math/MathF.h"


namespace
{
	using namespace Alpha;

	void TraverseRecursive(SceneObject* object, const std::function<bool(SceneObject* object)>& func)
	{
		if (!object)
			return;

		func(object);

		for (auto& child : object->GetChildren())
		{
			TraverseRecursive(child, func);
		}
	}

	bool IsDescendant(SceneObject* object, SceneObject* new_parent)
	{


		return true;
	}
}

namespace Alpha
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	void Scene::Initialize()
	{
	}

	void Scene::Update(float delta_time)
	{
		for (auto& root : m_root_objects)
		{
			TraverseRecursive(root, [&](SceneObject* object)->bool
			{
				auto& transform = m_registry.get<TransformComponent>(object->GetEntity());

				auto parent = object->GetParent();
				if (parent)
				{
					auto& parent_transform = m_registry.get<TransformComponent>(parent->GetEntity());

					transform.SetWorldMatrix(MatrixMultiply(transform.GetLocalMatrix(), parent_transform.GetWorldMatrix()));

					return false;
				}

				transform.SetWorldMatrix(transform.GetLocalMatrix());

				return false;

			});
		}

		CompactHierarchy();

	}

	SceneObject& Scene::CreateObject(const String& name)
	{
		const entt::entity entity = m_registry.create();

		auto object = std::make_unique<SceneObject>(this, entity, 0, name);

		SceneObject& result = *object;

		m_objects.push_back(std::move(object));
		m_root_objects.push_back(&result);

		m_registry.emplace<TransformComponent>(entity);

		return result;
	}

	SceneObject* Scene::FindObject(StringView name)
	{
		for (auto& object : m_objects)
		{
			if (object->GetName() == name)
				return object.get();
		}

		return nullptr;
	}

	bool Scene::Reparent(SceneObject* object, SceneObject* new_parent)
	{
		if (object->m_scene != this)
			return false;

		if (new_parent && new_parent->m_scene != this)
			return false;

		if (new_parent == object)
			return false;

		if (new_parent && IsDescendant(object, new_parent))
			return false;

		DetachFromHierarchy(object);

		object->m_parent = new_parent;

		if (new_parent)
		{
			new_parent->m_children.push_back(object);
		}
		else
		{
			m_root_objects.push_back(object);
		}

		return true;
	}

	void Scene::DetachFromHierarchy(SceneObject* object)
	{
		if (!object)
			return;

		auto& siblings = object->m_parent ? object->m_parent->m_children : m_root_objects;

		siblings[object->m_hierarchy_index] = nullptr;

		object->m_hierarchy_index = 0;

		object->m_parent = nullptr;
	}

	void Scene::CompactHierarchy()
	{
		std::erase(m_root_objects, nullptr);

		for (auto& object : m_objects)
		{
			std::erase(object->m_children, nullptr);
		}
	}

};