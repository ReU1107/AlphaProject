#pragma once
#include "Core/Types.h"

#include <functional>

namespace Alpha
{
	class RenderCore;
	class DescriptorTable;
	class DescriptorHeap;
	class StructuredBuffer;
	class ByteAddressBuffer;
	class PipelineLayout;
	class RingBuffer;
}

namespace Alpha::Particle
{
	struct EmitInfo
	{
		float4 color{ Color4::Russet };
		float life_time{ 0.0f };
		uint32_t max_num{ 0 };
	};

	struct EmitContext;

	class Emission
	{
	private:

	public:
		struct Settings
		{
			RenderCore* render_core{ nullptr };
			PipelineLayout* pipeline_layout{ nullptr };

		};


		virtual void Setup(const Settings& settings) = 0;
		virtual void Cleanup() = 0;

		virtual void Emit(EmitContext& ctxt) = 0;

	};

	class EmissionRegistry
	{
	public:
		// モジュールの型を登録しておくリスト
		using Creator = std::function<TUnique<Emission>()>;

		static TArray<Creator>& GetRegistry() 
		{
			static TArray<Creator> s_registry;
			return s_registry;
		}

		static void SetupAll(const Emission::Settings& settings)
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