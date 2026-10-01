#include "bmpch.hpp"
#include "Scene.hpp"

#include "Entity.hpp"

namespace BM
{
	Scene::Scene() noexcept
	{
		OnDestroy<Hierarchy>().connect<&Scene::RemoveEntityHierarchy>(this);
	}

	Registry& Scene::GetRegistry() noexcept
	{
		return m_Registry;
	}

	void Scene::OnEvent(Event& event) noexcept
	{
		for (auto& system : m_Systems)
			system.OnEvent(*this, event);
	}

	void Scene::OnTick(float timeStep) noexcept
	{
		for (auto& system : m_Systems)
			system.OnTick(*this, timeStep);
	}

	void Scene::OnUpdate(float deltaTime) noexcept
	{
		for (auto& system : m_Systems)
			system.OnUpdate(*this, deltaTime);
	}

	void Scene::OnRender() noexcept
	{
		for (auto& system : m_Systems)
			system.OnRender(*this);
	}

	Entity Scene::CreateEntity(const Component::Transform::LocalSpace& transform) noexcept
	{
		Entity entity = GetEntity(m_Registry.create());
		entity.Add<Component::Transform>(transform);

		BM_CORE_FN("Entity created (entity: '{}')", entity);

		return entity;
	}

	Entity Scene::CreateEntityWithParent(EntityHandle parentHandle, const Component::Transform::LocalSpace& transform) noexcept
	{
		Entity entity = CreateEntity(transform);
		AssignEntityParent(entity, parentHandle);

		return entity;
	}

	Entity Scene::GetEntity(EntityHandle handle) noexcept
	{
		return Entity(this, handle);
	}

	std::optional<Entity> Scene::GetEntityParent(EntityHandle handle) noexcept
	{
		if (!HasAllComponent<Hierarchy>(handle))
			return std::nullopt;

		return GetEntity(GetComponent<Hierarchy>(handle).Parent);
	}

	std::vector<Entity> Scene::GetEntityChildren(EntityHandle handle) noexcept
	{
		std::vector<Entity> children;
		if (!HasAllComponent<Hierarchy>(handle))
			return children;

		const auto& childrenHandles = GetComponent<Hierarchy>(handle).Children;
		children.reserve(childrenHandles.size());
		for (EntityHandle childHandle : childrenHandles)
			children.emplace_back(this, childHandle);

		return children;
	}

	void Scene::AssignEntityParent(EntityHandle handle, EntityHandle parentHandle) noexcept
	{
		if (!IsValid(handle) || !IsValid(parentHandle))
			return;

		if (GetEntityParent(handle))
			RemoveEntityParent(handle);

		AddOrReplaceComponent<Hierarchy>(handle, Hierarchy{ .Parent = parentHandle });

		AddOrGetComponent<Hierarchy>(parentHandle);
		PatchComponent<Hierarchy>(parentHandle, [&](auto& parentHierarchy) { parentHierarchy.Children.push_back(handle); });

		BM_CORE_FN("Entity got assigned a parent (entity: '{}', parent: '{}')", handle, parentHandle);
	}

	void Scene::RemoveEntityChildren(EntityHandle handle) noexcept
	{
		BM_CORE_FN_ARGS(handle);

		if (!HasAllComponent<Hierarchy>(handle))
			return;

		/*
		*  We copy the std::vector because every time we call 'Destroy', it also calls 'RemoveEntityHierarchy'
		*  which calls 'RemoveEntityParent' and removes the element during the foreach, which is probably UB.
		*  TLDR: Don't make this std::vector a reference.
		*/
		const std::vector cChildren = GetComponent<Hierarchy>(handle).Children;
		for (EntityHandle childHandle : cChildren)
		{
			if (IsValid(childHandle))
				Destroy(childHandle);
		}
	}

	void Scene::RemoveEntityParent(EntityHandle handle) noexcept
	{
		BM_CORE_FN_ARGS(handle);

		if (!HasAllComponent<Hierarchy>(handle))
			return;

		EntityHandle parentHandle = GetComponent<Hierarchy>(handle).Parent;
		if (IsValid(parentHandle))
		{
			PatchComponent<Hierarchy>(handle, [](Hierarchy& childHierarchy) {
				childHierarchy.Parent = entt::null;
				});

			if (HasAllComponent<Hierarchy>(parentHandle))
			{
				PatchComponent<Hierarchy>(parentHandle, [handle](Hierarchy& parentHierarchy) {
					std::erase(parentHierarchy.Children, handle);
					});
			}
		}
	}

	void Scene::RemoveEntityHierarchy(EntityHandle handle) noexcept
	{
		BM_CORE_FN_ARGS(handle);

		RemoveEntityChildren(handle);
		RemoveEntityParent(handle);
	}

	void Scene::ClearEntities() noexcept
	{
		BM_CORE_FN("Scene entities is being cleared");

		m_Registry.clear();

		BM_CORE_DEBUG_FN("Scene entities cleared");
	}

	void Scene::ClearContext() noexcept
	{
		BM_CORE_FN("Scene context is being cleared");

		m_Registry.ctx().clear();

		BM_CORE_DEBUG_FN("Scene context cleared");
	}

	void Scene::Clear() noexcept
	{
		BM_CORE_FN("Scene is being cleared");

		ClearEntities();
		ClearContext();

		BM_CORE_DEBUG_FN("Scene cleared");
	}

	void Scene::Destroy(EntityHandle handle) noexcept
	{
		BM_CORE_FN_ARGS(handle);
		m_Registry.destroy(handle);
	}

	bool Scene::IsValid(EntityHandle handle) const noexcept
	{
		return m_Registry.valid(handle);
	}

	void Scene::AttachRenderer(std::weak_ptr<Renderer> renderer) noexcept
	{
		BM_CORE_FN("Scene is attaching a renderer");
		AddOrReplaceCtxComponent<RendererComponent>(RendererComponent{ std::move(renderer) });
	}

	std::weak_ptr<Renderer> Scene::GetRenderer() noexcept
	{
		if (auto rendererComponent = TryGetCtxComponent<RendererComponent>())
			return rendererComponent->Renderer;

		return {};
	}
}
