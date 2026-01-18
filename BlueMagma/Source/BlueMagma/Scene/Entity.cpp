#include "bmpch.hpp"
#include "Entity.hpp"

namespace BM
{
	Entity::Entity(Scene* scene, EntityHandle handle) noexcept
		: m_ScenePtr(scene), m_Handle(handle)
	{
	}

	Entity::operator EntityHandle() const noexcept
	{
		return m_Handle;
	}

	Entity::operator bool() const noexcept
	{
		return static_cast<uint32_t>(m_Handle) != 0xFFFFFFFFu;
	}

	Entity Entity::CreateChild(const Component::Transform& transform) noexcept
	{
		Entity child = m_ScenePtr->Create(transform);
		child.Add<Parent>(m_Handle);

		return child;
	}
}
