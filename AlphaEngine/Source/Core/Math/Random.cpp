#include "Random.h"

namespace Alpha
{
	Random::Random()
		: m_randomDevice()
		, m_mt(m_randomDevice())
	{
	}

	Random::~Random()
	{
	}

	void Random::SetSeed(int64_t seed)
	{
		m_mt.seed(seed);
	}
}