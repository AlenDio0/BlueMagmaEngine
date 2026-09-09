#pragma once
#include <cstdint>
#include <random>

namespace BM
{
	class Random
	{
	public:
		explicit Random(uint64_t seed = std::random_device{}()) noexcept;

		void SetSeed(uint64_t seed) noexcept;

		uint64_t Generate(uint64_t min, uint64_t max) noexcept;
		double GenerateDouble(double min, double max) noexcept;
	public:
		static Random& Get() noexcept;
	private:
		std::mt19937_64 m_Engine;
	};
}

#define BM_RANDOM_SEED(seed)		BM::Random::Get().SetSeed(static_cast<uint64_t>(seed))
#define BM_RANDOM(min, max)			BM::Random::Get().Generate(static_cast<uint64_t>(min), static_cast<uint64_t>(max))
#define BM_RANDOM_DOUBLE(min, max)	BM::Random::Get().GenerateDouble(static_cast<double>(min), static_cast<double>(max))
