#include "GeometryUtil.h"
#include "Core/Literal.h"
#include "MathUtil.h"
#include "MathF.h"

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

    float SignedDistance(const Plane& plane, const Vector3& point)
    {
        return VectorDot(plane.normal, point) + plane.distance;
    }

    bool Intersect(const Frustum& frustum, const AxisAlignedBoundingBox& aabb)
    {
        const Vector3 center = aabb.GetCenter();
        const Vector3 extents = aabb.GetExtents();

        for (const Plane& plane : frustum.planes)
        {
            const Vector3 abs_normal = Vector3(
                Abs(plane.normal.X()),
                Abs(plane.normal.Y()),
                Abs(plane.normal.Z()));

            // AABBをPlaneの法線へ投影した半径
            const float radius = VectorDot(abs_normal, extents);

            const float distance = SignedDistance(plane, center);

            // AABB全体がPlaneの外側
            if (distance + radius < 0.0f)
            {
                return false;
            }
        }

        return true;
    }

    Frustum CreateFrustum(const Matrix4x4& inv_view_proj, const float near_z, const float far_z)
    {
        const Vector3 nlt = Vector3UnprojectNDC(Vector3(-1.0f, 1.0f, near_z), inv_view_proj);
        const Vector3 nrt = Vector3UnprojectNDC(Vector3(1.0f, 1.0f, near_z), inv_view_proj);
        const Vector3 nlb = Vector3UnprojectNDC(Vector3(-1.0f, -1.0f, near_z), inv_view_proj);
        const Vector3 nrb = Vector3UnprojectNDC(Vector3(1.0f, -1.0f, near_z), inv_view_proj);

        const Vector3 flt = Vector3UnprojectNDC(Vector3(-1.0f, 1.0f, far_z), inv_view_proj);
        const Vector3 frt = Vector3UnprojectNDC(Vector3(1.0f, 1.0f, far_z), inv_view_proj);
        const Vector3 flb = Vector3UnprojectNDC(Vector3(-1.0f, -1.0f, far_z), inv_view_proj);
        const Vector3 frb = Vector3UnprojectNDC(Vector3(1.0f, -1.0f, far_z), inv_view_proj);

        // 判定用の中心点
        const Vector3 center = (nlt + nrt + nlb + nrb) * 0.25f;

        Frustum result;

        result.planes[+EFrustumSide::Left] = CreatePlane(nlt, nlb, flb, center);
        result.planes[+EFrustumSide::Right] = CreatePlane(nrb, nrt, frt, center);
        result.planes[+EFrustumSide::Top] = CreatePlane(nrt, nlt, flt, center);
        result.planes[+EFrustumSide::Bottom] = CreatePlane(nlb, nrb, frb, center);
        result.planes[+EFrustumSide::Near] = CreatePlane(nlt, nrt, nrb, center);
        result.planes[+EFrustumSide::Far] = CreatePlane(frt, flt, flb, center);

        return result;
    }

    Plane CreatePlane(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& inside_point)
    {
        Plane result;

        result.normal = VectorNormalize(Vector3Cross(b - a, c - a));

        result.distance = -VectorDot(result.normal, a);

        // 内側が負ならPlaneを反転
        if (SignedDistance(result, inside_point) < 0.0f)
        {
            result.normal = -result.normal;
            result.distance = -result.distance;
        }

        return result;
    }

    AxisAlignedBoundingBox TransformAABB(const AxisAlignedBoundingBox& local_aabb, const Matrix4x4& world)
    {
        const Vector3 corners[8] =
        {
            { local_aabb.min.X(), local_aabb.min.Y(), local_aabb.min.Z() },
            { local_aabb.max.X(), local_aabb.min.Y(), local_aabb.min.Z() },
            { local_aabb.min.X(), local_aabb.max.Y(), local_aabb.min.Z() },
            { local_aabb.max.X(), local_aabb.max.Y(), local_aabb.min.Z() },

            { local_aabb.min.X(), local_aabb.min.Y(), local_aabb.max.Z() },
            { local_aabb.max.X(), local_aabb.min.Y(), local_aabb.max.Z() },
            { local_aabb.min.X(), local_aabb.max.Y(), local_aabb.max.Z() },
            { local_aabb.max.X(), local_aabb.max.Y(), local_aabb.max.Z() },
        };

        AxisAlignedBoundingBox result;
        result.min = Vector3(FLT_MAX);
        result.max = Vector3(-FLT_MAX);

        for (const Vector3& corner : corners)
        {
            const Vector3 transformed = Vector3Transform(corner, world);

            result.min = VectorMin(result.min, transformed);
            result.max = VectorMax(result.max, transformed);
        }

        return result;
    }

}