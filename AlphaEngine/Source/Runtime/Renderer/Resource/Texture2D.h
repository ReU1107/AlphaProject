#pragma once
#include "Core/Types.h"

namespace Alpha
{
	class Texture;
}

namespace Alpha::Renderer
{
	class Texture2D
	{
	private:
		TUnique<Texture> m_gpu_resource{};

	public:
		~Texture2D();

		void SetResource(TUnique<Texture> resource);

		Texture* GetResource() const { return m_gpu_resource.get(); }

	};

}