#pragma once
#include "Core/Common.h"
#include <entt/entt.hpp>

namespace Alpha
{
    class Scene;

    class SceneObject
    {
        friend class Scene;

    private:
        Scene* m_scene{ nullptr };

        UInt64 m_uuid{};
        String m_name{};


        SceneObject* m_parent{ nullptr };
        TArray<SceneObject*> m_children{};
        entt::entity m_entity{ entt::null };

    public:
        SceneObject(Scene* scene, entt::entity entity, UInt64 uuid, StringView name);

        void AddChild(SceneObject* child);

        void SetParent(SceneObject* parent);

        void Unparent();

        template<class T>
        T& AddComponent();

        template<class T>
        T& GetComponent();

        SceneObject* GetParent() const { return m_parent; }

        const TArray<SceneObject*>& GetChildren() const { return m_children; }

        entt::entity GetEntity() const { return m_entity; }
        const String& GetName() const { return m_name; }

    private:
        entt::registry& GetRegistry();

    };

    template<class T>
    inline T& SceneObject::AddComponent()
    {
        return GetRegistry().emplace<T>(m_entity);
    }

    template<class T>
    inline T& SceneObject::GetComponent()
    {
        return GetRegistry().get<T>(m_entity);
    }

}