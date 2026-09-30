#pragma once
#include "Core/Common.h"

namespace Alpha::Renderer
{
	struct LightComponent
	{
		float3 direction{};		// Transformからもらうやつ
		UInt32 type{ 0 };		// ライト種別 (0:Point, 1:Spot, 2:Directional)

	private:
		float3 m_color{};
		float m_radius{ 1.0f };
		float m_intensity{ 1.0f };

		float m_inner_cone{ 1.0f };	// スポットライト内角のcos
		float m_outer_cone{ 0.7f };	// スポットライト外角のcos

	public:

        void SetColor(const float3& color) { m_color = color; }
        float3 GetColor() const { return m_color; }

        void SetIntensity(float intensity) { m_intensity = intensity; }
        float GetIntensity() const { return m_intensity; }

        void SetRadius(float radius) { m_radius = radius;}
        float GetRadius() const { return m_radius; }

        void SetSpotAngleDegree(float inner_cone, float outer_cone);
        void SetSpotAngleRadian(float inner_cone, float outer_cone);

        float GetInnerCone() const { return m_inner_cone; }
        float GetOuterCone() const { return m_outer_cone; }

	};

	// タグ用
	struct MainLight
	{
		UInt32 index{ 0 };
	};

}
