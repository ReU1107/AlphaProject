#include "Texture2D.h"
#include "RenderCore/Texture/Texture.h"

namespace Alpha::Renderer
{
	Texture2D::~Texture2D() = default;

	void Texture2D::SetResource(TUnique<Texture> resource)
	{
		m_gpu_resource = std::move(resource);
	}

}