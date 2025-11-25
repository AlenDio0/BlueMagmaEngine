#pragma once

namespace BM
{
	namespace Random
	{
		size_t Get() noexcept;
		size_t Get(size_t min, size_t max) noexcept;
		double GetDouble() noexcept;
	}
}

#define BM_RANDOM(...) BM::Random::Get(__VA_ARGS__)
#define BM_RANDOM_DOUBLE() BM::Random::GetDouble()
