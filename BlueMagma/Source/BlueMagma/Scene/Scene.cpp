#include "bmpch.hpp"
#include "Scene.hpp"

#include "Entity.hpp"

namespace BM
{
	static inline void RemoveChild(Registry& registry, EntityHandle child) noexcept {
		BM_CORE_FN_ARGS(child);

		EntityHandle parent = registry.get<Parent>(child).Handle;
		if (registry.valid(parent) && registry.all_of<Children>(parent))
		{
			auto& childList = registry.get<Children>(parent).Handles;
			std::erase(childList, child);
		}
	}

	static inline void DestroyChildren(Registry& registry, EntityHandle entity) noexcept {
		BM_CORE_FN_ARGS(entity);

		if (registry.all_of<Children>(entity))
		{
			std::vector<EntityHandle> children = registry.get<Children>(entity).Handles;
			for (auto child : children)
			{
				if (registry.valid(child))
					registry.destroy(child);
			}
		}
	}

	//======================================================================================

	Scene::Scene() noexcept
	{
		OnDestroy<Parent>().connect<RemoveChild>();
		OnDestroy<Children>().connect<DestroyChildren>();
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
		if (!HasAllComponent<Parent>(handle))
			return {};

		return GetEntity(GetComponent<Parent>(handle).Handle);
	}

	std::vector<Entity> Scene::GetEntityChildren(EntityHandle handle) noexcept
	{
		std::vector<Entity> children;
		if (!HasAllComponent<Children>(handle))
			return children;

		const auto& childrenHandles = GetComponent<Children>(handle).Handles;
		children.reserve(childrenHandles.size());
		for (EntityHandle childHandle : childrenHandles)
			children.emplace_back(this, childHandle);

		return children;
	}

	void Scene::AssignEntityParent(EntityHandle handle, EntityHandle parentHandle) noexcept
	{
		if (!IsValid(handle) || !IsValid(parentHandle))
			return;

		if (std::optional<Entity> actualParent = GetEntityParent(handle))
		{
			if (actualParent != parentHandle)
			{
				BM_CORE_WARN_FN("Entity already has a parent assigned, nothings changes (entity: '{}', actualParent: '{}', parent: '{}')",
					handle, actualParent.value(), parentHandle);
			}

			return;
		}

		AddComponent<Parent>(handle, parentHandle);
		AddOrGetComponent<Children>(parentHandle);
		PatchComponent<Children>(parentHandle, [&](auto& children) { children.Handles.push_back(handle); });

		BM_CORE_FN("Entity got assigned a parent (entity: '{}', parent: '{}')", handle, parentHandle);
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
