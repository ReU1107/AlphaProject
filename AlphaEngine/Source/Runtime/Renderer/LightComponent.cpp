#include "LightComponent.h"
#include "Core/Math/MathUtil.h"

namespace Alpha::Renderer
{
    void LightComponent::SetSpotAngleDegree(float inner_cone, float outer_cone)
    {
		SetSpotAngleRadian(ConvertDegreeToRadian(inner_cone), ConvertDegreeToRadian(outer_cone));
    }

    void LightComponent::SetSpotAngleRadian(float inner_cone, float outer_cone)
    {
        m_inner_cone = Cos(inner_cone * 0.5f);
        m_outer_cone = Cos(outer_cone * 0.5f);
    }
}