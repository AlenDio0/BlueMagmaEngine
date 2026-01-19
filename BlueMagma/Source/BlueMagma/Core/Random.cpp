#include "bmpch.hpp"
#include "Random.hpp"
#include <random>
#include <ctime>
#include <limits>

namespace BM
{
	namespace Random
	{
		static inline struct RandomData
		{
			std::default_random_engine _Engine;
			std::uniform_int_distribution<size_t> _Distribution;

			inline RandomData() noexcept {
				_Engine.seed(static_cast<uint32_t>(time(nullptr)));
			}
		} s_Random;

		size_t Get() noexcept
		{
			return s_Random._Distribution(s_Random._Engine);
		}

		size_t Get(size_t min, size_t max) noexcept
		{
			BM_CORE_ASSERT(min <= max);
			return min + (Get() % (max - min + 1ull));
		}

		double GetDouble() noexcept
		{
			return static_cast<double>(Get()) / static_cast<double>(std::numeric_limits<size_t>::max());
		}
	}
}
