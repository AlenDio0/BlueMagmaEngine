#include "bmpch.hpp"
#include "TransformSystem.hpp"
#include "Scene/Scene.hpp"
#include <entt/entity/fwd.hpp>

namespace BM
{
	using namespace Component;

	static inline void SetTrue(bool& b) noexcept
	{
		b = true;
	}

	static inline void SetNeedUpdate(Registry& registry, EntityHandle entity) noexcept
	{
		auto& transform = registry.get<Transform>(entity);
		transform.CachedUpdated = false;
	}

	void TransformSystem::OnAttach(Scene& scene) noexcept
	{
		scene.OnConstruct<Transform>().connect<SetTrue>(m_CachedUpdatedZ);
		scene.OnDestroy<Transform>().connect<SetTrue>(m_CachedUpdatedZ);
		scene.OnUpdate<Transform>().connect<SetTrue>(m_CachedUpdatedZ);

		scene.OnUpdate<Transform>().connect<SetNeedUpdate>();
	}

	void TransformSystem::OnUpdate(Scene& scene, float deltaTime) noexcept
	{
		static constexpr Transform::GlobalSpace sRootGlobal{ .Position{0.f}, .Scale{1.f}, .Z = 0.f };

		auto view = scene.GetRegistry().view<Transform>(entt::exclude<Parent>);
		for (auto entity : view)
			UpdateTransformAndChildren(scene.GetEntity(entity), sRootGlobal, true);

		UpdateSortByZ(scene);
	}

	void TransformSystem::UpdateTransformAndChildren(Entity entity, const Transform::GlobalSpace& parentGlobal, bool updated) noexcept
	{
		auto& transform = entity.GetScene()->GetRegistry().get<Transform>(entity);
		auto& global = transform.Global;

		if (!transform.CachedUpdated || !updated)
		{
			const auto& local = transform.Local;

			global.Position = parentGlobal.Position + (local.State.Position * parentGlobal.Scale);
			global.Scale = parentGlobal.Scale * local.State.Scale;
			global.Z = parentGlobal.Z + local.Z;
		}

		auto children = entity.GetChildren<Transform>();
		for (auto child : children)
		{
			if (child)
				UpdateTransformAndChildren(child, global, transform.CachedUpdated);
		}

		transform.CachedUpdated = true;
	}

	void TransformSystem::UpdateSortByZ(Scene& scene) noexcept
	{
		if (m_CachedUpdatedZ)
		{
			scene.GetRegistry().sort<Transform>([](const auto& left, const auto& right) {
				return left.Global.Z < right.Global.Z; });

			m_CachedUpdatedZ = false;
		}
	}
}
