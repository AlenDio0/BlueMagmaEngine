#pragma once
#include "Math/Vec2.hpp"
#include "Math/Transform2D.hpp"

namespace BM::Component
{
	struct Transform
	{
		struct LocalSpace
		{
			Transform2D State{};
			float Z = 0.f;
		} Local;

		struct GlobalSpace
		{
			Vec2f Position{ 0.f };
			Vec2f Scale{ 1.f };
			float Rotation = 0.f;
			float Z = 0.f;
		} Global;

		bool CachedUpdated = false;
	};

	struct Hidden
	{
		bool Visible = false;
	};
}
