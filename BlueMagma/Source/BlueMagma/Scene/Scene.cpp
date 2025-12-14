#include "bmpch.hpp"
#include "Scene.hpp"
#include "Entity.hpp"

namespace BM
{
	Entity Scene::Create() noexcept
	{
		return Entity(this, m_Registry.create());
	}

	Entity Scene::GetEntity(EntityHandle handle) noexcept
	{
		return Entity(this, handle);
	}
}
