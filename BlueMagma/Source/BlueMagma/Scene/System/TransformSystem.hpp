#pragma once
#include "ISystem.hpp"

namespace BM
{
	class TransformSystem : public ISystem
	{
	public:
		virtual void OnAttach(Scene& scene) noexcept override;

		virtual void OnUpdate(Scene& scene, float deltaTime) noexcept override;
	private:
		static void UpdatePosition(Scene& scene) noexcept;
		static void UpdateZ(Scene& scene) noexcept;
		static void UpdateChildren(Scene& scene) noexcept;
	};
}
