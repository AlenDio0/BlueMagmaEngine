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
		void UpdatePosition(Scene& scene) noexcept;
		void UpdateZ(Scene& scene) noexcept;
		void UpdateChildren(Scene& scene) noexcept;
	private:
		bool m_UpdatedZ;
	};
}
