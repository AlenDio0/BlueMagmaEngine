#include "bmpch.hpp"
#include "TransformSystem.hpp"
#include "Scene/Entity.hpp"
#include "Scene/Scene.hpp"
#include <entt/entt.hpp>

namespace BM
{
	struct TransformState
	{
		bool _UpdatedZ = true;
	};

	static inline void SetTrue(bool& b) noexcept {
		b = true;
	}

	void TransformSystem::OnAttach(Scene& scene) noexcept
	{
		auto& state = scene.AddCtxComponent<TransformState>();

		using Transform = Component::Transform;
		scene.OnConstruct<Transform>().connect<SetTrue>(state._UpdatedZ);
		scene.OnUpdate<Transform>().connect<SetTrue>(state._UpdatedZ);
	}

	void TransformSystem::OnUpdate(Scene& scene, float deltaTime) noexcept
	{
		UpdatePosition(scene);
		UpdateZ(scene);
		UpdateChildren(scene);
	}

	void TransformSystem::UpdatePosition(Scene& scene) noexcept
	{
		auto view = scene.GetRegistry().view<Component::Transform>(entt::exclude<Parent>);
		for (auto entity : view)
		{
			scene.PatchComponent<Component::Transform>(entity, [](auto& transform) {
				transform._Position = transform._LocalPosition;
				});
		}
	}

	void TransformSystem::UpdateZ(Scene& scene) noexcept
	{
		auto& state = scene.GetCtxComponent<TransformState>();

		if (state._UpdatedZ)
		{
			scene.GetRegistry().sort<Component::Transform>([](const auto& left, const auto& right) {
				return left._Z < right._Z; });

			state._UpdatedZ = false;
		}
	}

	void TransformSystem::UpdateChildren(Scene& scene) noexcept
	{
		auto children = scene.View<Component::Transform, Parent>();
		for (auto [child, _, parent] : children.each())
		{
			const auto& parentTransform = scene.GetComponent<Component::Transform>(parent._ParentHandle);
			scene.PatchComponent<Component::Transform>(child, [&](auto& transform) {
				transform._Position = parentTransform._Position + transform._LocalPosition;
				});
		}
	}
}
