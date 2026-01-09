#pragma once
#include "Core/Assert.hpp"
#include "Component.hpp"
#include <entt/entt.hpp>
#include <entt/entity/fwd.hpp>

namespace BM
{
	class Entity;
	using EntityHandle = entt::entity;

	class Scene
	{
	public:
		Scene() noexcept;

		Entity Create(const Component::Transform& transform = {}) noexcept;

		Entity GetEntity(EntityHandle handle) noexcept;

		void OnUpdate() noexcept;
		void OnRender(sf::RenderTarget& target) const noexcept;

		template<class... TComp>
		inline decltype(auto) View() const noexcept {
			return m_Registry.view<TComp...>();
		}

		template<class... TComp>
		inline bool HasComponent(EntityHandle handle) const noexcept {
			return m_Registry.all_of<TComp...>(handle);
		}
		template<class TComp>
		inline const TComp& GetComponent(EntityHandle handle) const noexcept {
			BM_CORE_ASSERT(HasComponent<TComp>(handle), "Entity doesn't have TComp");
			return m_Registry.get<TComp>(handle);
		}
		template<class TComp, typename... Args>
		inline const TComp& AddComponent(EntityHandle handle, Args&&... args) noexcept {
			BM_CORE_ASSERT(!HasComponent<TComp>(handle), "Entity already has TComp");
			return m_Registry.emplace<TComp>(handle, std::forward<Args>(args)...);
		}
		template<class TComp, typename... Args>
		inline const TComp& ReplaceComponent(EntityHandle handle, Args&&... args) noexcept {
			BM_CORE_ASSERT(HasComponent<TComp>(handle), "Entity doesn't have TComp");
			return m_Registry.replace<TComp>(handle, std::forward<Args>(args)...);
		}
		template<class TComp, class... Funcs>
		inline const TComp& PatchComponent(EntityHandle handle, Funcs&&... funcs) noexcept {
			BM_CORE_ASSERT(HasComponent<TComp>(handle), "Entity doesn't have TComp");
			return m_Registry.patch<TComp>(handle, std::forward<Funcs>(funcs)...);
		}
		template<class TComp>
		inline void RemoveComponent(EntityHandle handle) noexcept {
			BM_CORE_ASSERT(HasComponent<TComp>(handle), "Entity doesn't have TComp");
			m_Registry.remove<TComp>(handle);
		}

		template<class TComp>
		inline const TComp* TryGetComponent(EntityHandle handle) const noexcept {
			return m_Registry.try_get<TComp>(handle);
		}
		template<class TComp, typename... Args>
		inline const TComp& AddOrGetComponent(EntityHandle handle, Args&&... args) noexcept {
			return m_Registry.get_or_emplace<TComp>(handle, std::forward<Args>(args)...);
		}
		template<class TComp, typename... Args>
		inline const TComp& AddOrReplaceComponent(EntityHandle handle, Args&&... args) noexcept {
			return m_Registry.emplace_or_replace<TComp>(handle, std::forward<Args>(args)...);
		}
	private:
		void OnTransformUpdate() noexcept;
	private:
		entt::registry m_Registry;

		bool m_UpdateTransform = true;
	};
}
