#include "bmpch.hpp"
#include "Random.hpp"

namespace BM
{
	Random::Random(uint32_t seed) noexcept
	{
		SetSeed(seed);
	}

	void Random::SetSeed(uint32_t seed) noexcept
	{
		m_Engine.seed(seed);
	}

	size_t Random::Generate(size_t min, size_t max) noexcept
	{
		std::uniform_int_distribution<size_t> distribution{ min, max };
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
