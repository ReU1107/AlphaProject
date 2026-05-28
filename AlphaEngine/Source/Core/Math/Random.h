#pragma once
#include <random>
#include <cstdint>

// メルセンヌツイスター

namespace Alpha
{
	class Random
	{
	private:
		std::random_device m_randomDevice;
		std::mt19937_64 m_mt;
	public:
		// コンストラクタ
		Random();
		// デストラクタ
		~Random();

	public:
		template<class T>
		T GetRandom(T min, T max);

		template<class T>
		T GetRandom();

		void SetSeed(int64_t seed);
	};

	template<class T>
	inline T Random::GetRandom(T min, T max)
	{
		std::uniform_real_distribution<T> random(min, max);
		return random(m_mt);
	}

	template<>
	inline int32_t Random::GetRandom(int32_t min, int32_t max)
	{
		std::uniform_int_distribution<int32_t> random(min, max);
		return random(m_mt);
	}

	template<class T>
	inline T Random::GetRandom()
	{
		std::uniform_real_distribution<T> random;
		return random(m_mt);
	}

}