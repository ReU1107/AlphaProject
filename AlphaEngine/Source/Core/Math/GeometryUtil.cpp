#include "GeometryUtil.h"
#include "Core/Literal.h"

namespace Alpha
{
    void CreateUnitCube(TArray<Vector3>* positions, TArray<Vector3>* normals, TArray<Vector2>* texcoords, TArray<UInt32>* indices)
    {
        if (positions)
        {
            positions->clear();
            *positions = {
                // 前面 (Z+)
                {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f},
                // 背面 (Z-)
                { 0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f},
                // 上面 (Y+)
                {-0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f},
                // 下面 (Y-)
                {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f},
                // 右面 (X+)
                { 0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f},
                // 左面 (X-)
                {-1.0f * 0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f}
            };
        }

        if (normals)
        {
            normals->clear();
            *normals = {
                // 前面 (Z+)
                {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
                // 背面 (Z-)
                {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
                // 上面 (Y+)
                {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
                // 下面 (Y-)
                {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
                // 右面 (X+)
                {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
                // 左面 (X-)
                {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}
            };
        }

        if (texcoords)
        {
            texcoords->clear();
            *texcoords = {
                // 前面 (Z+)
                {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
                // 背面 (Z-)
                {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
                // 上面 (Y+)
                {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
                // 下面 (Y-)
                {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
                // 右面 (X+)
                {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
                // 左面 (X-)
                {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}
            };
        }

        if (indices)
        {
            indices->clear();
            for (UInt32 i = 0; i < 6; ++i)
            {
                UInt32 offset = i * 4;

                indices->push_back(offset + 0);
                indices->push_back(offset + 2);
                indices->push_back(offset + 1);

                indices->push_back(offset + 0);
                indices->push_back(offset + 3);
                indices->push_back(offset + 2);
            }
        }
    }

    void CreateUnitSphere(UInt32 statck_count, UInt32 sector_count, TArray<Vector3>* positions, TArray<Vector3>* normals, TArray<Vector2>* texcoords, TArray<UInt32>* indices)
    {
        // 昔のコード引っ張ってきた
        const float radius = 1.0f;
        const float sector_step = PI2 / sector_count;
        const float stack_step = PI / statck_count;
        const float inverse_length = 1.0f / radius;

        for (UInt32 i = 0; i <= statck_count; ++i)
        {
            const float stackAngle = PIDiv2 - i * stack_step;

            for (UInt32 j = 0; j <= sector_count; ++j)
            {
                const float sectorAngle = j * sector_step;

                float x = radius * cosf(stackAngle);
                float y = radius * sinf(stackAngle);

                Vector3 position = Vector3::Zero;
                position.X() = x * sinf(sectorAngle);
                position.Y() = y;
                position.Z() = x * cosf(sectorAngle);

                Vector3 normal = Vector3::Zero;
                normal.X() = position.X() * inverse_length;
                normal.Y() = position.Y() * inverse_length;
                normal.Z() = position.Z() * inverse_length;

                Vector2 texcoord = Vector2::Zero;
                texcoord.X() = (float)j / sector_count;
                texcoord.Y() = (float)i / statck_count;

                if (positions)
                    positions->push_back(position);
                if (normals)
                    normals->push_back(normal);
                if (texcoords)
                    texcoords->push_back(texcoord);
            }

        }

        if (!indices)
            return;

        UInt32 k1 = 0;
        UInt32 k2 = 0;

        for (UInt32 i = 0; i < statck_count; ++i)
        {
            k1 = i * (sector_count + 1);
            k2 = k1 + sector_count + 1;

            for (UInt32 j = 0; j < sector_count; ++j, ++k1, ++k2)
            {
                if (i != 0)
                {
                    indices->push_back(k1);
                    indices->push_back(k2);
                    indices->push_back(k1 + 1);
                }

                if (i != (statck_count - 1))
                {
                    indices->push_back(k1 + 1);
                    indices->push_back(k2);
                    indices->push_back(k2 + 1);
                }
            }
        }
    }
}