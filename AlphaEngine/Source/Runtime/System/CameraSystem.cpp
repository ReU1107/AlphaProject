#include "CameraSystem.h"
#include "Renderer/CameraComponent.h"

#include "TransformComponent.h"

#include "Core/Math/MathUtil.h"
#include "Core/Math/MathF.h"

namespace Alpha
{
	void CameraSystem::Update(entt::registry& registry)
	{
        float dt = 1.0f / 60;

        float move_speed = 10.0f;
        float rotation_speed = 2.5f;

		auto view = registry.view<CameraComponent, TransformComponent>();

		for (auto [ent, camera, transform] : view.each())
		{
            static float cam_yaw = 0.0f; // 左右回転（ヨー）
            static float cam_pitch = 0.0f; // 上下回転（ピッチ）

            // キー入力で角度（数値）を直接操作する
            if (GetKeyState(VK_RIGHT) & 0x8000) { cam_yaw += rotation_speed * dt; }
            if (GetKeyState(VK_LEFT) & 0x8000) { cam_yaw -= rotation_speed * dt; }
            if (GetKeyState(VK_UP) & 0x8000) { cam_pitch -= rotation_speed * dt; }
            if (GetKeyState(VK_DOWN) & 0x8000) { cam_pitch += rotation_speed * dt; }

            if (cam_pitch > 1.55f) cam_pitch = 1.55f;
            if (cam_pitch < -1.55f) cam_pitch = -1.55f;

            Quaternion new_rotation = QuaternionRollPitchYaw(0.0f, cam_pitch, cam_yaw);
            transform.SetLocalRotation(new_rotation);

            Vector3 move_dir(0.0f, 0.0f, 0.0f);

            if (GetKeyState('W') & 0x8000) { move_dir += Matrix4x4Forward(transform.GetWorldMatrix()); }
            if (GetKeyState('S') & 0x8000) { move_dir += Matrix4x4Backward(transform.GetWorldMatrix()); }
            if (GetKeyState('A') & 0x8000) { move_dir += Matrix4x4Left(transform.GetWorldMatrix()); }
            if (GetKeyState('D') & 0x8000) { move_dir += Matrix4x4Right(transform.GetWorldMatrix()); }
            if (GetKeyState('Z') & 0x8000) { move_dir += Vector3::YAxis; }
            if (GetKeyState('X') & 0x8000) { move_dir += -Vector3::YAxis; }

            if (VectorLength(move_dir) > 0.0f)
            {
                move_dir = VectorNormalize(move_dir);
                Vector3 displacement = move_dir * move_speed * dt;

                transform.LocalTranslate(displacement);
            }

            camera.view_matrix = Matrix4x4Inverse(transform.GetWorldMatrix());
		}

	}
}
