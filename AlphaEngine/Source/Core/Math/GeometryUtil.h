#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"
#include "AxisAlignedBoundingBox.h"
#include "Frustum.h"
#include "Core/Container/Array.h"

namespace Alpha
{
    void CreateUnitCube(TArray<Vector3>* positions, TArray<Vector3>* normals, TArray<Vector2>* texcoords, TArray<UInt32>* indices);

    void CreateUnitSphere(UInt32 stackCount, UInt32 sectorCount, TArray<Vector3>* positions, TArray<Vector3>* normals, TArray<Vector2>* texcoords, TArray<UInt32>* indices);

    float SignedDistance(const Plane& plane, const Vector3& point);


    bool Intersect(const Frustum& frustum, const AxisAlignedBoundingBox& aabb);

    Frustum CreateFrustum(const Matrix4x4& inv_view_proj, const float near_z, const float far_z);

    Plane CreatePlane(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& inside_point);

    AxisAlignedBoundingBox TransformAABB(const AxisAlignedBoundingBox& local_aabb, const Matrix4x4& world);

}