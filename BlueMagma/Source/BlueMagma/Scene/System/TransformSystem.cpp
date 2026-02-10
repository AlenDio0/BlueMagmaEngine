#include "bmpch.hpp"
#include "TransformSystem.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include <entt/entt.hpp>

namespace BM
{
	static inline void SetTrue(bool& b) noexcept {
		b = true;
	}

	void TransformSystem::OnAttach(Scene& scene) noexcept
	{
		using Transform = Component::Transform;
		scene.OnConstruct<Transform>().connect<SetTrue>(m_UpdatedZ);
		scene.OnUpdate<Transform>().connect<SetTrue>(m_UpdatedZ);
	}

	void TransformSystem::OnUpdate(Scene& scene, float deltaTime) noexcept
	{
		UpdateTransform(scene);
		UpdateChildren(scene);

		UpdateSortByZ(scene);
	}

	void TransformSystem::UpdateTransform(Scene& scene) noexcept
	{
		auto view = scene.GetRegistry().view<Component::Transform>(entt::exclude<Parent>);
		for (auto [entity, transform] : view.each())
		{
			transform._Position = transform._LocalPosition;
			transform._Z = transform._LocalZ;
			transform._Scale = transform._LocalScale;
		}
	}

	void TransformSystem::UpdateSortByZ(Scene& scene) noexcept
	{
		if (m_UpdatedZ)
		{
			scene.GetRegistry().sort<Component::Transform>([](const auto& left, const auto& right) {
				return left._Z < right._Z; });

			m_UpdatedZ = false;
		}
	}

	void TransformSystem::UpdateChildren(Scene& scene) noexcept
	{
		auto children = scene.GetRegistry().view<Component::Transform, Parent>();
		for (auto [child, transform, parent] : children.each())
		{
			const auto& cParentTransform = scene.GetRegistry().get<Component::Transform>(parent._ParentHandle);

			transform._Position = cParentTransform._Position + (transform._LocalPosition * cParentTransform._Scale);
			transform._Z = cParentTransform._Z + transform._LocalZ;
			transform._Scale = cParentTransform._Scale * transform._LocalScale;
		}
	}
}
