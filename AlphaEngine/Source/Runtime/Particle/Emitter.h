#pragma once
#include "Core/Object.h"
#include "Context.h"

namespace Alpha
{
	class MaterialHandle;
}

namespace Alpha::Particle
{
	class Emission;
	class Behavior;

	class Emitter : public Object
	{
	private:
		TUnique<Emission> m_emission{ nullptr };
		TUniqueArray<Behavior> m_behaviors{};

		TUnique<StructuredBuffer> m_particle_pool{};
		TUnique<ByteAddressBuffer> m_dead_list{};

		TUnique<DescriptorTable> m_table{};
		
		Vector4 m_base_color{ Color4::Magenta };
		float m_spawn_rate{ 1.0f };			// 
		float m_spawn_accumulator{ 0.0f };	// 
		float m_life_time{ 5.0f };			// 
		uint32_t m_max_num{ 0 };

	public:
		Emitter();
		~Emitter();

		struct Args
		{
			RenderCore* core{ nullptr };
			DescriptorHeap* heap{ nullptr };
			uint32_t max_count{ 512 };
		};

		void Initialize(const Args& args);

		void SetSpawnRate(float rate) { m_spawn_rate = rate; }

		void SetLifeTime(float time) { m_life_time = time; }

		void SetEmission(TUnique<Emission> emission);

		// 追加された順番で処理されます
		void AddBehavior(TUnique<Behavior> behavior);

		void Update(Context& ctx);

		void Play();

		void Stop();

		void Pause();

		void Clear();

		void Accept(IVisitor* visitor) override;

	};

}