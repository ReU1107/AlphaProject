#pragma once
#include "Core/Types.h"

namespace Alpha::Renderer
{
	class Mesh;
	class StructuredBuffer;

	class MeshInstance
	{
	private:
		TShared<Mesh> m_mesh{ nullptr };
		TUnique<StructuredBuffer> m_bone_matrix_buffer{ nullptr };

	public:
		~MeshInstance();
		void SetMesh(const TShared<Mesh>& mesh);



	};

}