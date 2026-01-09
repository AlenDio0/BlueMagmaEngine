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
		inline const TComp& Get() const noexcept {
			return m_ScenePtr->GetComponent<TComp>(m_Handle);
		}
		template<class TComp, typename... Args>
		inline const TComp& Add(Args&&... args) noexcept {
			return m_ScenePtr->AddComponent<TComp>(m_Handle, std::forward<Args>(args)...);
		}
		template<class TComp, typename... Args>
		inline const TComp& Replace(Args&&... args) noexcept {
			return m_ScenePtr->ReplaceComponent<TComp>(m_Handle, std::forward<Args>(args)...);
		}
		template<class TComp, class... Funcs>
		inline const TComp& Patch(Funcs&&... funcs) noexcept {
			return m_ScenePtr->PatchComponent<TComp>(m_Handle, std::forward<Funcs>(funcs)...);
		}
		template<class TComp>
		inline void Remove() noexcept {
			m_ScenePtr->RemoveComponent<TComp>(m_Handle);
		}

		template<class TComp>
		inline const TComp* TryGet() const noexcept {
			return m_ScenePtr->TryGetComponent<TComp>(m_Handle);
		}
		template<class TComp, typename... Args>
		inline const TComp& AddOrGet(Args&&... args) noexcept {
			return m_ScenePtr->AddOrGetComponent<TComp>(m_Handle, std::forward<Args>(args)...);
		}
		template<class TComp, typename... Args>
		inline const TComp& AddOrReplace(Args&&... args) noexcept {
			return m_ScenePtr->AddOrReplaceComponent<TComp>(m_Handle, std::forward<Args>(args)...);
		}
	private:
		Scene* m_ScenePtr = nullptr;
		EntityHandle m_Handle{ 0xFFFFFFFFu };
	};
}