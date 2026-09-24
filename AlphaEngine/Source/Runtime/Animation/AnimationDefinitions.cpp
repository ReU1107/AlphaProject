#include "Definitions.h"
#include "Core/Math/MathF.h"

namespace Alpha::Animation
{
    void Snapshot::Blend(const Snapshot& lhs, const Snapshot& rhs, float alpha)
    {
        // Transform 補間
        for (size_t i = 0; i < lhs.bone_transforms.size(); ++i)
        {
            auto& lhs_tf = lhs.bone_transforms[i];
            auto& rhs_tf = rhs.bone_transforms[i];

            bone_transforms[i].position = VectorLerp(lhs_tf.position, rhs_tf.position, alpha);
            bone_transforms[i].quaternion = QuaternionSlerp(lhs_tf.quaternion, rhs_tf.quaternion, alpha);
            bone_transforms[i].scale = VectorLerp(lhs_tf.scale, rhs_tf.scale, alpha);
        }

    }
}