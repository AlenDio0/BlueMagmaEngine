#include "bmpch.hpp"
#include "Scene.hpp"
#include "Entity.hpp"

namespace BM
{
	entt::registry& Scene::GetRegistry() noexcept
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
}
