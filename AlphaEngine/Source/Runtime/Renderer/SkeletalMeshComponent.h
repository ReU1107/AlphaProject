#pragma once
#include "Core/Component.h"

namespace Alpha
{
	class RingBuffer;
	namespace Renderer
	{
		class SkeletalMesh;
	}

	class SkeletalMeshComponent// : public Component
	{
	public:
		TShared<Renderer::SkeletalMesh> m_mesh{};      // 何を描くか
		//TShared<const Skeleton>     m_skeleton;  // どの骨構造か
		TUnique<RingBuffer>   m_bone_buffer; // 今の骨の行列（GPU転送用）

	public:
		~SkeletalMeshComponent();

	};
}