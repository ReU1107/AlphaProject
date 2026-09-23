#include "ParticleEmitterComponent.h"

#include "Core/Math/MathUtil.h"

namespace Alpha::Particle
{
    void ParticleEmitterComponent::SetShapePoint()
    {
        source_shape = SourceShape::Point;
        distribution = EmitDistribution::Center;
        shape_params = float4(0.0f);
    }

    void ParticleEmitterComponent::SetShapeLineVertex(float length)
    {
        source_shape = SourceShape::Line;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(length, 0.0f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeLineEdge(float length)
    {
        source_shape = SourceShape::Line;
        distribution = EmitDistribution::Edge;
        shape_params = float4(length, 0.0f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangleVertex(const float2& size)
    {
        source_shape = SourceShape::Triangle;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(size, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangleEdge(const float2& size)
    {
        source_shape = SourceShape::Triangle;
        distribution = EmitDistribution::Edge;
        shape_params = float4(size, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangleSurface(const float2& size)
    {
        source_shape = SourceShape::Triangle;
        distribution = EmitDistribution::Surface;
        shape_params = float4(size, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRectangleVertex(const float2& size)
    {
        source_shape = SourceShape::Rectangle;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(size * 0.5f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRectangleEdge(const float2& size)
    {
        source_shape = SourceShape::Rectangle;
        distribution = EmitDistribution::Edge;
        shape_params = float4(size * 0.5f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRectangleSurface(const float2& size)
    {
        source_shape = SourceShape::Rectangle;
        distribution = EmitDistribution::Surface;
        shape_params = float4(size * 0.5f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeCircleEdge(float radius)
    {
        source_shape = SourceShape::Circle;
        distribution = EmitDistribution::Edge;
        shape_params = float4(radius, 0.0f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeCircleSurface(float radius)
    {
        source_shape = SourceShape::Circle;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radius, 0.0f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeEllipseEdge(const float2& radii)
    {
        source_shape = SourceShape::Ellipse;
        distribution = EmitDistribution::Edge;
        shape_params = float4(radii, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeEllipseSurface(const float2& radii)
    {
        source_shape = SourceShape::Ellipse;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radii, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeAnnulusEdge(float inner_radius, float outer_radius)
    {
        source_shape = SourceShape::Annulus;
        distribution = EmitDistribution::Edge;
        shape_params = float4(inner_radius, outer_radius, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeAnnulusSurface(float inner_radius, float outer_radius)
    {
        source_shape = SourceShape::Annulus;
        distribution = EmitDistribution::Surface;
        shape_params = float4(inner_radius, outer_radius, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeArcVertex(float radius, float angle_degrees)
    {
        source_shape = SourceShape::Arc;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(radius, angle_degrees, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeArcEdge(float radius, float angle_degrees)
    {
        source_shape = SourceShape::Arc;
        distribution = EmitDistribution::Edge;
        shape_params = float4(radius, angle_degrees, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeSectorEdge(float radius, float angle_degrees)
    {
        source_shape = SourceShape::Sector;
        distribution = EmitDistribution::Edge;
        shape_params = float4(radius, angle_degrees, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeSectorSurface(float radius, float angle_degrees)
    {
        source_shape = SourceShape::Sector;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radius, angle_degrees, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPolygonVertex(float radius, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPolygon;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(radius, static_cast<float>(side_count), 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPolygonEdge(float radius, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPolygon;
        distribution = EmitDistribution::Edge;
        shape_params = float4(radius, static_cast<float>(side_count), 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPolygonSurface(float radius, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPolygon;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radius, static_cast<float>(side_count), 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeBoxVertex(const float3& size)
    {
        source_shape = SourceShape::Box;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(size * 0.5f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeBoxEdge(const float3& size)
    {
        source_shape = SourceShape::Box;
        distribution = EmitDistribution::Edge;
        shape_params = float4(size * 0.5f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeBoxSurface(const float3& size)
    {
        source_shape = SourceShape::Box;
        distribution = EmitDistribution::Surface;
        shape_params = float4(size * 0.5f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeBoxVolume(const float3& size)
    {
        source_shape = SourceShape::Box;
        distribution = EmitDistribution::Volume;
        shape_params = float4(size * 0.5f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRoundedBoxSurface(const float3& size, float corner_radius)
    {
        source_shape = SourceShape::RoundedBox;
        distribution = EmitDistribution::Surface;
        shape_params = float4(size, corner_radius);
    }

    void ParticleEmitterComponent::SetShapeRoundedBoxVolume(const float3& size, float corner_radius)
    {
        source_shape = SourceShape::RoundedBox;
        distribution = EmitDistribution::Volume;
        shape_params = float4(size, corner_radius);
    }

    void ParticleEmitterComponent::SetShapeSphereSurface(float radius)
    {
        source_shape = SourceShape::Sphere;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radius, 0.0f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeSphereVolume(float radius)
    {
        source_shape = SourceShape::Sphere;
        distribution = EmitDistribution::Volume;
        shape_params = float4(radius, 0.0f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeHemisphereSurface(float radius)
    {
        source_shape = SourceShape::Hemisphere;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radius, 0.0f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeHemisphereVolume(float radius)
    {
        source_shape = SourceShape::Hemisphere;
        distribution = EmitDistribution::Volume;
        shape_params = float4(radius, 0.0f, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeEllipsoidSurface(const float3& radii)
    {
        source_shape = SourceShape::Ellipsoid;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radii, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeEllipsoidVolume(const float3& radii)
    {
        source_shape = SourceShape::Ellipsoid;
        distribution = EmitDistribution::Volume;
        shape_params = float4(radii, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeConeSurface(float radius, float height)
    {
        source_shape = SourceShape::Cone;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radius, height, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeConeVolume(float radius, float height)
    {
        source_shape = SourceShape::Cone;
        distribution = EmitDistribution::Volume;
        shape_params = float4(radius, height, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeCylinderEdge(float radius, float height)
    {
        source_shape = SourceShape::Cylinder;
        distribution = EmitDistribution::Edge;
        shape_params = float4(radius, height, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeCylinderSurface(float radius, float height)
    {
        source_shape = SourceShape::Cylinder;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radius, height, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeCylinderVolume(float radius, float height)
    {
        source_shape = SourceShape::Cylinder;
        distribution = EmitDistribution::Volume;
        shape_params = float4(radius, height, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangularPyramidVertex(const float3& size)
    {
        source_shape = SourceShape::TriangularPyramid;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(size, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangularPyramidEdge(const float3& size)
    {
        source_shape = SourceShape::TriangularPyramid;
        distribution = EmitDistribution::Edge;
        shape_params = float4(size, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangularPyramidSurface(const float3& size)
    {
        source_shape = SourceShape::TriangularPyramid;
        distribution = EmitDistribution::Surface;
        shape_params = float4(size, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangularPyramidVolume(const float3& size)
    {
        source_shape = SourceShape::TriangularPyramid;
        distribution = EmitDistribution::Volume;
        shape_params = float4(size, 0.0f);
    }

    void ParticleEmitterComponent::SetShapePyramidVertex(const float2& base_size, float height)
    {
        source_shape = SourceShape::Pyramid;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(base_size, height, 0.0f);
    }

    void ParticleEmitterComponent::SetShapePyramidEdge(const float2& base_size, float height)
    {
        source_shape = SourceShape::Pyramid;
        distribution = EmitDistribution::Edge;
        shape_params = float4(base_size, height, 0.0f);
    }

    void ParticleEmitterComponent::SetShapePyramidSurface(const float2& base_size, float height)
    {
        source_shape = SourceShape::Pyramid;
        distribution = EmitDistribution::Surface;
        shape_params = float4(base_size, height, 0.0f);
    }

    void ParticleEmitterComponent::SetShapePyramidVolume(const float2& base_size, float height)
    {
        source_shape = SourceShape::Pyramid;
        distribution = EmitDistribution::Volume;
        shape_params = float4(base_size, height, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPyramidVertex(float radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPyramid;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(radius, height, static_cast<float>(side_count), 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPyramidEdge(float radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPyramid;
        distribution = EmitDistribution::Edge;
        shape_params = float4(radius, height, static_cast<float>(side_count), 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPyramidSurface(float radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPyramid;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radius, height, static_cast<float>(side_count), 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPyramidVolume(float radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPyramid;
        distribution = EmitDistribution::Volume;
        shape_params = float4(radius, height, static_cast<float>(side_count), 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangularPrismVertex(const float2& triangle_size, float depth)
    {
        source_shape = SourceShape::TriangularPrism;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(triangle_size, depth, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangularPrismEdge(const float2& triangle_size, float depth)
    {
        source_shape = SourceShape::TriangularPrism;
        distribution = EmitDistribution::Edge;
        shape_params = float4(triangle_size, depth, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangularPrismSurface(const float2& triangle_size, float depth)
    {
        source_shape = SourceShape::TriangularPrism;
        distribution = EmitDistribution::Surface;
        shape_params = float4(triangle_size, depth, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTriangularPrismVolume(const float2& triangle_size, float depth)
    {
        source_shape = SourceShape::TriangularPrism;
        distribution = EmitDistribution::Volume;
        shape_params = float4(triangle_size, depth, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPrismVertex(float radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPrism;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(radius, height, static_cast<float>(side_count), 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPrismEdge(float radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPrism;
        distribution = EmitDistribution::Edge;
        shape_params = float4(radius, height, static_cast<float>(side_count), 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPrismSurface(float radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPrism;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radius, height, static_cast<float>(side_count), 0.0f);
    }

    void ParticleEmitterComponent::SetShapeRegularPrismVolume(float radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPrism;
        distribution = EmitDistribution::Volume;
        shape_params = float4(radius, height, static_cast<float>(side_count), 0.0f);
    }

    void ParticleEmitterComponent::SetShapeCapsuleSurface(float radius, float cylinder_height)
    {
        source_shape = SourceShape::Capsule;
        distribution = EmitDistribution::Surface;
        shape_params = float4(radius, cylinder_height, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeCapsuleVolume(float radius, float cylinder_height)
    {
        source_shape = SourceShape::Capsule;
        distribution = EmitDistribution::Volume;
        shape_params = float4(radius, cylinder_height, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTorusSurface(float major_radius, float minor_radius)
    {
        source_shape = SourceShape::Torus;
        distribution = EmitDistribution::Surface;
        shape_params = float4(major_radius, minor_radius, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTorusVolume(float major_radius, float minor_radius)
    {
        source_shape = SourceShape::Torus;
        distribution = EmitDistribution::Volume;
        shape_params = float4(major_radius, minor_radius, 0.0f, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTubeSurface(float inner_radius, float outer_radius, float height)
    {
        source_shape = SourceShape::Tube;
        distribution = EmitDistribution::Surface;
        shape_params = float4(inner_radius, outer_radius, height, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeTubeVolume(float inner_radius, float outer_radius, float height)
    {
        source_shape = SourceShape::Tube;
        distribution = EmitDistribution::Volume;
        shape_params = float4(inner_radius, outer_radius, height, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeConeFrustumSurface(float bottom_radius, float top_radius, float height)
    {
        source_shape = SourceShape::ConeFrustum;
        distribution = EmitDistribution::Surface;
        shape_params = float4(bottom_radius, top_radius, height, 0.0f);
    }

    void ParticleEmitterComponent::SetShapeConeFrustumVolume(float bottom_radius, float top_radius, float height)
    {
        source_shape = SourceShape::ConeFrustum;
        distribution = EmitDistribution::Volume;
        shape_params = float4(bottom_radius, top_radius, height, 0.0f);
    }

    void ParticleEmitterComponent::SetShapePyramidFrustumVertex(const float2& bottom_size, float top_scale, float height)
    {
        source_shape = SourceShape::PyramidFrustum;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(bottom_size, top_scale, height);
    }

    void ParticleEmitterComponent::SetShapePyramidFrustumEdge(const float2& bottom_size, float top_scale, float height)
    {
        source_shape = SourceShape::PyramidFrustum;
        distribution = EmitDistribution::Edge;
        shape_params = float4(bottom_size, top_scale, height);
    }

    void ParticleEmitterComponent::SetShapePyramidFrustumSurface(const float2& bottom_size, float top_scale, float height)
    {
        source_shape = SourceShape::PyramidFrustum;
        distribution = EmitDistribution::Surface;
        shape_params = float4(bottom_size, top_scale, height);
    }

    void ParticleEmitterComponent::SetShapePyramidFrustumVolume(const float2& bottom_size, float top_scale, float height)
    {
        source_shape = SourceShape::PyramidFrustum;
        distribution = EmitDistribution::Volume;
        shape_params = float4(bottom_size, top_scale, height);
    }

    void ParticleEmitterComponent::SetShapeRegularPyramidFrustumVertex(float bottom_radius, float top_radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPyramidFrustum;
        distribution = EmitDistribution::Vertex;
        shape_params = float4(bottom_radius, top_radius, height, static_cast<float>(side_count));
    }

    void ParticleEmitterComponent::SetShapeRegularPyramidFrustumEdge(float bottom_radius, float top_radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPyramidFrustum;
        distribution = EmitDistribution::Edge;
        shape_params = float4(bottom_radius, top_radius, height, static_cast<float>(side_count));
    }

    void ParticleEmitterComponent::SetShapeRegularPyramidFrustumSurface(float bottom_radius, float top_radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPyramidFrustum;
        distribution = EmitDistribution::Surface;
        shape_params = float4(bottom_radius, top_radius, height, static_cast<float>(side_count));
    }

    void ParticleEmitterComponent::SetShapeRegularPyramidFrustumVolume(float bottom_radius, float top_radius, float height, UInt32 side_count)
    {
        source_shape = SourceShape::RegularPyramidFrustum;
        distribution = EmitDistribution::Volume;
        shape_params = float4(bottom_radius, top_radius, height, static_cast<float>(side_count));
    }
}