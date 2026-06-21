#pragma once
#include "AssetHandle.h" 

/*
* 今はModelHandleの中で持っているSkeletonの実体を、将来的にここに引っ越しさせます
*/

namespace Alpha
{
    namespace Animation
    {
        class Skeleton;
    }

    class SkeletonHandle : public AssetHandle
    {
    private:
        TUnique<Animation::Skeleton> m_skeleton{};

    public:
        ~SkeletonHandle();

        static AssetType Type() { return AssetType::Skeleton; }
        AssetType GetType() const final { return AssetType::Skeleton; }

        void SetSkeleton(TUnique<Animation::Skeleton>&& skeleton)
        {
            m_skeleton = std::move(skeleton);
        }

        const Animation::Skeleton* GetSkeleton() const { return m_skeleton.get(); }
        Animation::Skeleton* GetSkeleton() { return m_skeleton.get(); }
    };
}