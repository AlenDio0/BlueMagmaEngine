#include "bmpch.hpp"
#include "Utils.hpp"

namespace BM::Utils
{
	float InverseLerp(float current, float from, float to) noexcept
	{
		if (to - from == 0.f)
			return 0.f;

		return (current - from) / (to - from);
	}
}
