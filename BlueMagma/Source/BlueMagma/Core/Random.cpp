#include "bmpch.hpp"
#include "Random.hpp"

namespace BM
{
	Random::Random(uint64_t seed) noexcept
	{
		SetSeed(seed);
	}

	void Random::SetSeed(uint64_t seed) noexcept
	{
		m_Engine.seed(seed);
	}

	uint64_t Random::Generate(uint64_t min, uint64_t max) noexcept
	{
		std::uniform_int_distribution<uint64_t> distribution{ min, max };
		return distribution(m_Engine);
	}

	double Random::GenerateDouble(double min, double max) noexcept
	{
		std::uniform_real_distribution<double> distribution{ min, max };
		return distribution(m_Engine);
	}

	Random& Random::Get() noexcept
	{
		static Random sRandom{};
		return sRandom;
	}
}
