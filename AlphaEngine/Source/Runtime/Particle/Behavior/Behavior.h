#pragma once
#include "Core/Types.h"
#include <functional>

namespace Alpha
{
	class RenderCore;
	class PipelineLayout;
}

namespace Alpha::Particle
{
	struct UpdateContext;

	class Behavior
	{
	public:
		virtual ~Behavior() = default;
		struct Settings
		{
			RenderCore* render_core{ nullptr };
			PipelineLayout* pipeline_layout{ nullptr };

		};
		virtual void Setup(const Settings& settings) = 0;
		virtual void Cleanup() = 0;

		virtual void Execute(UpdateContext& ctx) = 0;

	};


	class BehaviorRegistry
	{
	public:
		// モジュールの型を登録しておくリスト
		using Creator = std::function<TUnique<Behavior>()>;

		static TArray<Creator>& GetRegistry()
		{
			static TArray<Creator> s_registry;
			return s_registry;
		}

		static void SetupAll(const Behavior::Settings& settings)
		{
			for (auto& create : GetRegistry())
			{
				auto instance = create();
				instance->Setup(settings);
			}
		}

		//static void CleanupAll()
		//{
		//	for (auto& create : GetRegistry())
		//	{
		//		instance->Cleanup();
		//	}
		//}

	};

}