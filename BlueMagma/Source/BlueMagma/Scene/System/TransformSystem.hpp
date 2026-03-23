#pragma once
#include "ISystem.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Component/Base.hpp"

namespace BM
{
	class TransformSystem : public ISystem
	{
	public:
		virtual void OnAttach(Scene& scene) noexcept override;

		virtual void OnUpdate(Scene& scene, float deltaTime) noexcept override;
	private:
		void UpdateTransformAndChildren(Entity entity, const Component::Transform::GlobalSpace& parentGlobal, bool updated) noexcept;
		void UpdateSortByZ(Scene& scene) noexcept;
	private:
		bool m_CachedUpdatedZ = true;
	};
}
