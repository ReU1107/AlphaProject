#include "IKSolver.h"
#include "SkeletonPose.h"

#include "Skeleton.h"
#include "Joint.h"
#include "IKChain.h"

#include "Core/Math/MathF.h"

#include <ranges>

namespace Alpha::Animation
{
	void IKSolver::Solve(SkeletonPose* pose)
	{
		// FABRIK https://qiita.com/tland_p/items/e57077192b3bcecebac2
		// このsolverが使用するchain
		auto skeleton = pose->GetSkeleton();
		IKChain* chain = skeleton->GetIKChain(m_chain_index);

        auto name = skeleton->GetJoint(chain->GetTargetIndex())->GetName();

        Vector3 target_pos = pose->GetWorldPosition(chain->GetTargetIndex());

        // 座標配列の作成(根元から{エンドエフェクターから遠い順}格納していく)
        TArray<Vector3> positions{};
        for (auto& link : chain->GetLinks() | std::views::reverse)
        {
            Vector3 current_pos = pose->GetWorldPosition(link.index);
            positions.push_back(current_pos);
        }
        // 一番最後にエフェクター(エンドポイント)
        positions.push_back(pose->GetWorldPosition(chain->GetEffectorIndex()));

        // 根元の固定位置を保存
        Vector3 origin_pos = positions[0];

        m_iteration_count = chain->GetMaxIterationCount();

        for (uint32_t iter = 0; iter < m_iteration_count; ++iter)
        {
            // 目標地点との距離が十分近ければ終了
            if (VectorLengthSq(positions.back() - target_pos) < Epsilon) 
                break;

            // エンドポイントをターゲットポジションに移動
            positions.back() = target_pos;

            // backward(qiitaの奴のa～d)
            for (int i = positions.size() - 2; i >= 0; --i)
            {
                const auto& link = chain->GetLink(i);
                float length = link.length;
                // i + 1 と i番目の直線上に射影
                Vector3 solved_dir = VectorNormalize(positions[i] - positions[i + 1]);
                Vector3 dir = solved_dir;

                positions[i] = positions[i + 1] + dir * length;
            }

            // 根元を計算前の座標に
            positions[0] = origin_pos;

            // forward
            for (int i = 0; i < positions.size() - 1; ++i) 
            {
                const auto& link = chain->GetLink(i);
                float length = link.length;
                /*
                * iが右足(足の付け根)としたら
                * i+1は膝
                */
                Vector3 solved_dir = VectorNormalize(positions[i + 1] - positions[i]);
                Vector3 dir = solved_dir;

                if (link.constraint.mode == EConstraintMode::Hinge)
                {
                    Quaternion parent_rot = Quaternion::Identity;
                    if (i > 0)
                    {
                        const auto& prev_link = chain->GetLink(i - 1);
                        Vector3 prev_dir = VectorNormalize(positions[i] - positions[i - 1]);
                        // 「バインド時の親の向き」から「今の親の向き」への回転を求める
                        parent_rot = QuaternionRotationFromTo(prev_link.dir, prev_dir);
                    }

                    Vector3 hinge_axis = Vector3TransformDir(link.constraint.rotation_axis, parent_rot);
                    Vector3 base_dir = Vector3TransformDir(link.dir, parent_rot);

                    // 平面に射影
                    dir = VectorNormalize(solved_dir - (VectorDot(solved_dir, hinge_axis) * hinge_axis));

                    float angle = ATan2(VectorDot(Vector3Cross(base_dir, dir), hinge_axis), VectorDot(base_dir, dir));
                    float clamped_angle = Clamp(angle, link.constraint.min_angle, link.constraint.max_angle);

                    // 改めてclampした値で初期状態から回転させる
                    dir = Vector3TransformDir(base_dir, QuaternionRotationAxis(hinge_axis, clamped_angle));
                }

                positions[i + 1] = positions[i] + dir * length;

            }
        }
        
        TArray<Vector3> old_positions;

        for (auto& link : chain->GetLinks() | std::views::reverse)
        {
            old_positions.push_back(pose->GetWorldPosition(link.index));
        }
        old_positions.push_back(pose->GetWorldPosition(chain->GetEffectorIndex()));

        // 根元から書き戻していく
        for (const auto& [i, link] : chain->GetLinks() | std::views::enumerate | std::views::reverse)
        {
        
            Vector3 old_dir = VectorNormalize(old_positions[i + 1] - old_positions[i]);

            Vector3 new_dir = VectorNormalize(positions[i + 1] - positions[i]);

            Quaternion delta_rot = QuaternionRotationFromTo(old_dir, new_dir);

            Quaternion new_world_rot = delta_rot;

            auto parent = pose->GetSkeleton()->GetJoint(link.index)->GetParent();
            Quaternion local_rot;
            if (parent)
            {
                Quaternion parent_world_rot = pose->GetWorldRotation(parent->GetIndex());
                local_rot = QuaternionInverse(parent_world_rot) * new_world_rot;
            }
            else
            {
                local_rot = new_world_rot;
            }

            // ポーズに反映（これで次のループの GetWorldRotation が更新される）
            pose->SetLocalRotation(link.index, local_rot);

        }
    }
}