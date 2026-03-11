#include "bmpch.hpp"
#include "Scene.hpp"
#include "Entity.hpp"

namespace BM
{
	static inline void RemoveChild(Registry& registry, EntityHandle child) noexcept {
		BM_CORE_FN("child: {}", child);

		EntityHandle parent = registry.get<Parent>(child).Handle;
		if (registry.valid(parent) && registry.all_of<Children>(parent))
		{
			auto& childList = registry.get<Children>(parent).Handles;
			std::erase(childList, child);
		}
	}

	static inline void DestroyChildren(Registry& registry, EntityHandle entity) noexcept {
		BM_CORE_FN("entity: {}", entity);
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

	Entity Scene::CreateEntity(const Component::Transform& transform) noexcept
	{
		Entity entity{ this, m_Registry.create() };
		entity.Add<Component::Transform>(transform);

		return entity;
	}

	Entity Scene::GetEntity(EntityHandle handle) noexcept
	{
		return Entity(this, handle);
	}

	void Scene::Clear() noexcept
	{
		m_Registry.clear();
	}

	void Scene::Destroy(EntityHandle handle) noexcept
	{
		BM_CORE_FN("handle: {}", handle);
		m_Registry.destroy(handle);
	}

	bool Scene::IsValid(EntityHandle handle) const noexcept
	{
		return m_Registry.valid(handle);
	}

	void Scene::AttachRenderer(Renderer& renderer) noexcept
	{
		AddOrReplaceCtxComponent<Renderer*>(&renderer);
	}

	Renderer* Scene::GetRenderer() noexcept
	{
		auto renderer = TryGetCtxComponent<Renderer*>();
		if (!renderer)
			return nullptr;

		return *renderer;
	}
}
