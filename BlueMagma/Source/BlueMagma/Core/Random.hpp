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

#define BM_RANDOM_SEED(seed)	BM::Random::Get().SetSeed(seed)
#define BM_RANDOM(...)			BM::Random::Get().Generate(__VA_ARGS__)
#define BM_RANDOM_DOUBLE(...)	BM::Random::Get().GenerateDouble(__VA_ARGS__)
