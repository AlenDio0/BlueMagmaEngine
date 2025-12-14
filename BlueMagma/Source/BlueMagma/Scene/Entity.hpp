#pragma once
#include "Scene.hpp"

namespace BM
{
	class Entity
	{
	public:
		inline Entity() noexcept = default;
		inline Entity(const Entity&) noexcept = default;
		Entity(Scene* scene, EntityHandle handle) noexcept;
		virtual inline ~Entity() noexcept = default;

		operator EntityHandle() const noexcept;
		operator bool() const noexcept;

		template<class... TComp>
		inline bool Has() const noexcept {
			return m_ScenePtr->HasComponent<TComp...>(m_Handle);
		}
		template<class TComp>
		inline TComp& Get() const noexcept {
			return m_ScenePtr->GetComponent<TComp>(m_Handle);
		}
		template<class TComp, typename... Args>
		inline TComp& Add(Args&&... args) noexcept {
			return m_ScenePtr->AddComponent<TComp>(m_Handle, std::forward<Args>(args)...);
		}
		template<class TComp, typename... Args>
		inline TComp& AddOrGet(Args&&... args) noexcept {
			return m_ScenePtr->AddOrGetComponent<TComp>(m_Handle, std::forward<Args>(args)...);
		}
		template<class TComp, typename... Args>
		inline TComp& AddOrReplace(Args&&... args) noexcept {
			return m_ScenePtr->AddOrReplaceComponent<TComp>(m_Handle, std::forward<Args>(args)...);
		}
		template<class TComp>
		inline void Remove() noexcept {
			m_ScenePtr->RemoveComponent<TComp>(m_Handle);
		}
	private:
		Scene* m_ScenePtr = nullptr;
		EntityHandle m_Handle{ 0 };
	};
}