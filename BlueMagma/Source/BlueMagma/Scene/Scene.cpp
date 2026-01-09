#include "bmpch.hpp"
#include "Scene.hpp"
#include "Entity.hpp"

namespace BM
{
	static inline void Mark(bool& b) noexcept {
		b = true;
	}

	Scene::Scene() noexcept
	{
		m_Registry.on_construct<Component::Transform>().connect<&Mark>(m_UpdateTransform);
		m_Registry.on_update<Component::Transform>().connect<&Mark>(m_UpdateTransform);
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

	void Scene::OnUpdate() noexcept
	{
		if (m_UpdateTransform)
			OnTransformUpdate();
	}

	void Scene::OnRender(sf::RenderTarget& target) const noexcept
	{
		using namespace Component;

		{
			auto view = View<Transform>();
			for (auto entity : view)
			{
				const Transform& transform = GetComponent<Transform>(entity);

				if (auto rect = TryGetComponent<RectRender>(entity))
					rect->Draw(target, transform);
				if (auto circle = TryGetComponent<CircleRender>(entity))
					circle->Draw(target, transform);
				if (auto texture = TryGetComponent<TextureRender>(entity))
					texture->Draw(target, transform);
				if (auto text = TryGetComponent<TextRender>(entity))
					text->Draw(target, transform);
			}
		}
	}

	void Scene::OnTransformUpdate() noexcept
	{
		m_Registry.sort<Component::Transform>([](const auto& left, const auto& right) { return left._Z < right._Z; });
		m_UpdateTransform = false;
	}
}
