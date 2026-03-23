#pragma once
#include "Vec2.hpp"

namespace BM
{
	struct Transform2D
	{
		Vec2f Position{ 0.f };
		Vec2f Scale{ 1.f };
		Vec2f Origin{ 0.f };
	};
}
