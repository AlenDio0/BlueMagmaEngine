#include "bmpch.hpp"
#include "Entity.hpp"

namespace BM
{
	Entity::Entity(Scene* scene, EntityHandle handle) noexcept
		: m_ScenePtr(scene), m_Handle(handle)
	{
	}

	Scene* Entity::GetScene() noexcept
	{
		return m_ScenePtr;
	}

	const Scene* Entity::GetScene() const noexcept
	{
		return m_ScenePtr;
	}

	EntityHandle Entity::GetHandle() const noexcept
	{
		return m_Handle;
	}

	bool Entity::IsValid() const noexcept
	{
		if (!m_ScenePtr)
			return false;

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

	Entity Entity::CreateChild(const Component::Transform::LocalSpace& transform) noexcept
	{
		return m_ScenePtr->CreateEntityWithParent(m_Handle, transform);
	}

	void Entity::AssignParent(EntityHandle parentHandle) noexcept
	{
		m_ScenePtr->AssignEntityParent(m_Handle, parentHandle);
	}

	std::vector<Entity> Entity::GetChildren() noexcept
	{
		return m_ScenePtr->GetEntityChildren(m_Handle);
	}

	std::optional<Entity> Entity::GetParent() noexcept
	{
		return m_ScenePtr->GetEntityParent(m_Handle);
	}
}
