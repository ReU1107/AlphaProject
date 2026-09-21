#pragma once
#include "Core/Common.h"

namespace Alpha::Particle
{
    enum class SourceShape
    {
        // 0D / 1D

        Point,

        Line,


        // 2D

        Triangle,
        Rectangle,

        Circle,
        Ellipse,

        Annulus,            // 円環
        Arc,                // 円弧
        Sector,             // 扇形

        RegularPolygon,     // 正多角形

        // 3D

        Box,
        RoundedBox,

        Sphere,
        Hemisphere,
        Ellipsoid,

        Cone,
        Cylinder,

        TriangularPyramid,  // 三角錐
        Pyramid,            // 四角錐
        RegularPyramid,     // 正多角錐

        TriangularPrism,    // 三角柱
        RegularPrism,       // 正多角柱

        Capsule,

        Torus,

        Tube,               // 中空円柱

        ConeFrustum,        // 円錐台
        PyramidFrustum,     // 四角錐台
        RegularPyramidFrustum, // 正多角錐台


        Mesh,
        Spline,
    };

    enum class EmitDistribution
    {
        Center,  
        Vertex,  
        Edge,    
        Surface, 
        Volume,  
    };

    struct ParticleEmitterComponent
    {
        UInt32 max_particle_count{ 1000 };

        float emission_rate{ 100.0f };
        float lifetime{ 5.0f };             // 生成したパーティクル1つの生存時間

        float duration{ 1.0f };             // このエミッターが再生開始されてから

        float3 initial_velocity{ 0.0f };
        float3 velocity_random{ 0.0f };

        float4 start_color = float4::One;
        float4 end_color = float4::Zero;

        float start_size{ 0.1f };
        float end_size{ 0.0f };

        UInt32 random_seed{ 0 };

        SourceShape source_shape{ SourceShape::Point };
        EmitDistribution distribution{ EmitDistribution::Center };

        float4 shape_params{ 0.0f };

        float spawn_accumulator{ 0.0f };

        bool emitting{ true };

        void Start()
        {
            emitting = true;
        }

        void Stop()
        {
            emitting = false;
        }

        void SetShapePoint();

        // 線分
        void SetShapeLineVertex(float length);
        void SetShapeLineEdge(float length);

        // 三角形
        void SetShapeTriangleVertex(const float2& size);
        void SetShapeTriangleEdge(const float2& size);
        void SetShapeTriangleSurface(const float2& size);

        // 矩形
        void SetShapeRectangleVertex(const float2& size);
        void SetShapeRectangleEdge(const float2& size);
        void SetShapeRectangleSurface(const float2& size);

        // 円
        void SetShapeCircleEdge(float radius);
        void SetShapeCircleSurface(float radius);

        // 楕円
        void SetShapeEllipseEdge(const float2& radii);
        void SetShapeEllipseSurface(const float2& radii);

        // 環状
        void SetShapeAnnulusEdge(float inner_radius, float outer_radius);
        void SetShapeAnnulusSurface(float inner_radius, float outer_radius);

        // 円弧
        void SetShapeArcVertex(float radius, float angle_degrees);
        void SetShapeArcEdge(float radius, float angle_degrees);

        // 扇
        void SetShapeSectorEdge(float radius, float angle_degrees);
        void SetShapeSectorSurface(float radius, float angle_degrees);

        // 正多角形
        void SetShapeRegularPolygonVertex(float radius, UInt32 side_count);
        void SetShapeRegularPolygonEdge(float radius, UInt32 side_count);
        void SetShapeRegularPolygonSurface(float radius, UInt32 side_count);

        // 箱
        void SetShapeBoxVertex(const float3& size);
        void SetShapeBoxEdge(const float3& size);
        void SetShapeBoxSurface(const float3& size);
        void SetShapeBoxVolume(const float3& size);

        // 角丸直方体
        void SetShapeRoundedBoxSurface(const float3& size, float corner_radius);
        void SetShapeRoundedBoxVolume(const float3& size, float corner_radius);

        // 球
        void SetShapeSphereSurface(float radius);
        void SetShapeSphereVolume(float radius);

        // 半球
        void SetShapeHemisphereSurface(float radius);
        void SetShapeHemisphereVolume(float radius);

        // 楕円体
        void SetShapeEllipsoidSurface(const float3& radii);
        void SetShapeEllipsoidVolume(const float3& radii);

        // 円錐
        void SetShapeConeSurface(float radius, float height);
        void SetShapeConeVolume(float radius, float height);

        // 円柱
        void SetShapeCylinderEdge(float radius, float height);
        void SetShapeCylinderSurface(float radius, float height);
        void SetShapeCylinderVolume(float radius, float height);

        // 三角錐
        void SetShapeTriangularPyramidVertex(const float3& size);
        void SetShapeTriangularPyramidEdge(const float3& size);
        void SetShapeTriangularPyramidSurface(const float3& size);
        void SetShapeTriangularPyramidVolume(const float3& size);

        // 四角錐
        void SetShapePyramidVertex(const float2& base_size, float height);
        void SetShapePyramidEdge(const float2& base_size, float height);
        void SetShapePyramidSurface(const float2& base_size, float height);
        void SetShapePyramidVolume(const float2& base_size, float height);

        // 多角錐
        void SetShapeRegularPyramidVertex(float radius, float height, UInt32 side_count);
        void SetShapeRegularPyramidEdge(float radius, float height, UInt32 side_count);
        void SetShapeRegularPyramidSurface(float radius, float height, UInt32 side_count);
        void SetShapeRegularPyramidVolume(float radius, float height, UInt32 side_count);

        // 三角柱
        void SetShapeTriangularPrismVertex(const float2& triangle_size, float depth);
        void SetShapeTriangularPrismEdge(const float2& triangle_size, float depth);
        void SetShapeTriangularPrismSurface(const float2& triangle_size, float depth);
        void SetShapeTriangularPrismVolume(const float2& triangle_size, float depth);

        // 多角柱
        void SetShapeRegularPrismVertex(float radius, float height, UInt32 side_count);
        void SetShapeRegularPrismEdge(float radius, float height, UInt32 side_count);
        void SetShapeRegularPrismSurface(float radius, float height, UInt32 side_count);
        void SetShapeRegularPrismVolume(float radius, float height, UInt32 side_count);

        // 球柱
        void SetShapeCapsuleSurface(float radius, float cylinder_height);
        void SetShapeCapsuleVolume(float radius, float cylinder_height);

        // 円環面
        void SetShapeTorusSurface(float major_radius, float minor_radius);
        void SetShapeTorusVolume(float major_radius, float minor_radius);

        // 円筒管
        void SetShapeTubeSurface(float inner_radius, float outer_radius, float height);
        void SetShapeTubeVolume(float inner_radius, float outer_radius, float height);

        // 円錐台
        void SetShapeConeFrustumSurface(float bottom_radius, float top_radius, float height);
        void SetShapeConeFrustumVolume(float bottom_radius, float top_radius, float height);

        // 四錐台
        void SetShapePyramidFrustumVertex(const float2& bottom_size, float top_scale, float height);
        void SetShapePyramidFrustumEdge(const float2& bottom_size, float top_scale, float height);
        void SetShapePyramidFrustumSurface(const float2& bottom_size, float top_scale, float height);
        void SetShapePyramidFrustumVolume(const float2& bottom_size, float top_scale, float height);

        // 多角錐台
        void SetShapeRegularPyramidFrustumVertex(float bottom_radius, float top_radius, float height, UInt32 side_count);
        void SetShapeRegularPyramidFrustumEdge(float bottom_radius, float top_radius, float height, UInt32 side_count);
        void SetShapeRegularPyramidFrustumSurface(float bottom_radius, float top_radius, float height, UInt32 side_count);
        void SetShapeRegularPyramidFrustumVolume(float bottom_radius, float top_radius, float height, UInt32 side_count);
    };
}