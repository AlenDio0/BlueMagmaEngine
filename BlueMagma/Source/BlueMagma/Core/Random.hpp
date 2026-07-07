#pragma once
#include <cstdint>
#include <random>

namespace BM
{
	class Random
	{
	public:
		explicit Random(uint32_t seed = std::random_device{}()) noexcept;

		void SetSeed(uint32_t seed) noexcept;

		size_t Generate(size_t min, size_t max) noexcept;
		double GenerateDouble(double min, double max) noexcept;
	public:
		static Random& Get() noexcept;
	private:
		std::default_random_engine m_Engine;
	};
}

#define BM_RANDOM_SEED(seed)		BM::Random::Get().SetSeed(static_cast<uint32_t>(seed))
#define BM_RANDOM(min, max)			BM::Random::Get().Generate(static_cast<size_t>(min), static_cast<size_t>(max))
#define BM_RANDOM_DOUBLE(min, max)	BM::Random::Get().GenerateDouble(static_cast<double>(min), static_cast<double>(max))
