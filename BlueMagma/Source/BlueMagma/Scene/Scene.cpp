#include "bmpch.hpp"
#include "Scene.hpp"
#include "Entity.hpp"

namespace BM
{
	entt::registry& Scene::GetRegistry() noexcept
	{
		return m_Registry;
	}

	void Scene::AddSystemEvent(SystemEventFn onEvent) noexcept
	{
		m_SystemEvents.push_back(onEvent);
	}

	void Scene::AddSystemUpdate(SystemUpdateFn onUpdate) noexcept
	{
		m_SystemUpdates.push_back(onUpdate);
	}

	void Scene::AddSystemRender(SystemRenderFn onRender) noexcept
	{
		m_SystemRenders.push_back(onRender);
	}

	void Scene::OnEvent(Event& event) noexcept
	{
		for (auto& onEvent : m_SystemEvents)
			onEvent(*this, event);
	}

	void Scene::OnUpdate(float deltaTime) noexcept
	{
		for (auto& onUpdate : m_SystemUpdates)
			onUpdate(*this, deltaTime);
	}

	void Scene::OnRender(sf::RenderTarget& target) const noexcept
	{
		for (auto& onRender : m_SystemRenders)
			onRender(*this, target);
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
