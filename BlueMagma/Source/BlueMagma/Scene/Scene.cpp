#include "bmpch.hpp"
#include "Scene.hpp"
#include "Entity.hpp"

namespace BM
{
	static inline void RemoveChild(Registry& registry, EntityHandle child) noexcept {
		EntityHandle parent = registry.get<Parent>(child)._ParentHandle;

		if (registry.valid(parent) && registry.all_of<Children>(parent))
		{
			auto& childList = registry.get<Children>(parent)._Children;
			std::erase(childList, child);
		}
	}

	static inline void DestroyChildren(Registry& registry, EntityHandle entity) noexcept {
		if (registry.all_of<Children>(entity))
		{
			std::vector<EntityHandle> children = registry.get<Children>(entity)._Children;
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
			system._EventFn(*this, event);
	}

	void Scene::OnUpdate(float deltaTime) noexcept
	{
		for (auto& system : m_Systems)
			system._UpdateFn(*this, deltaTime);
	}

	void Scene::OnRender(sf::RenderTarget& target) const noexcept
	{
		for (auto& system : m_Systems)
			system._RenderFn(*this, target);
	}

	Entity Scene::Create(const Component::Transform& transform) noexcept
	{
		Entity entity{ this, m_Registry.create() };
		entity.Add<Component::Transform>(transform);

		return entity;
	}

	Entity Scene::GetEntity(EntityHandle handle) noexcept
	{
		return Entity(this, handle);
	}

	void Scene::Destroy(EntityHandle handle) noexcept
	{
		m_Registry.destroy(handle);
	}
}
