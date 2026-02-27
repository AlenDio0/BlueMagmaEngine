#include "bmpch.hpp"
#include "Entity.hpp"

namespace BM
{
	Entity::Entity(Scene* scene, EntityHandle handle) noexcept
		: m_ScenePtr(scene), m_Handle(handle)
	{
	}

	EntityHandle Entity::GetHandle() const noexcept
	{
		return m_Handle;
	}

	bool Entity::IsValid() const noexcept
	{
		return m_ScenePtr->IsValid(m_Handle);
	}

	Entity::operator EntityHandle() const noexcept
	{
		return m_Handle;
	}

	Entity::operator bool() const noexcept
	{
		return IsValid();
	}

	Entity Entity::CreateChild(const Component::Transform& transform) noexcept
	{
		Entity child = m_ScenePtr->Create(transform);
		child.Add<Parent>(m_Handle);

		AddOrGet<Children>();
		Patch<Children>([&](auto& children) { children.Handles.push_back(child); });

		return child;
	}

	std::vector<Entity> Entity::GetChildren() noexcept
	{
		std::vector<Entity> children;
		if (!Has<Children>())
			return children;

		const auto& childList = Get<Children>().Handles;
		for (auto& child : childList)
			children.emplace_back(m_ScenePtr, child);

		return children;
	}
}
