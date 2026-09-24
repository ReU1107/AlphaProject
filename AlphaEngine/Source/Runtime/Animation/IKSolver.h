#pragma once
#include "Core/Types.h"

namespace Alpha::Animation
{
	class SkeletonPose;

	class IKSolver
	{
	public:
		uint32_t m_chain_index{ 0 };

		float m_weight{};
		uint32_t m_iteration_count{};



	public:
		IKSolver() = default;
		~IKSolver() = default;

		void Solve(SkeletonPose* pose);

	};

}