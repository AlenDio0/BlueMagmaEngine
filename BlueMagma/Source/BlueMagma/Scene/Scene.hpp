#pragma once
#include <entt/entt.hpp>
#include <entt/entity/fwd.hpp>

namespace BM
{
	class Entity;
	using EntityHandle = entt::entity;

	class Scene
	{
	public:
		inline Scene() noexcept = default;
		virtual inline ~Scene() noexcept = default;

		Entity Create() noexcept;

		Entity GetEntity(EntityHandle handle) noexcept;

		template<class... TComp>
		inline decltype(auto) View() const noexcept {
			return m_Registry.view<TComp...>();
		}
		template<class... TComp>
		inline decltype(auto) Group() const noexcept {
			return m_Registry.group<TComp...>();
		}

		template<class... TComp>
		inline bool HasComponent(EntityHandle handle) const noexcept {
			return m_Registry.all_of<TComp...>(handle);
		}
		template<class TComp>
		inline TComp& GetComponent(EntityHandle handle) const noexcept {
			BM_CORE_ASSERT(Has<TComp>(handle), "Entity doesn't have TComp");
			return m_Registry.get<TComp>(handle);
		}
		template<class TComp, typename... Args>
		inline TComp& AddComponent(EntityHandle handle, Args&&... args) noexcept {
			BM_CORE_ASSERT(!Has<TComp>(handle), "Entity already has TComp");
			return m_Registry.emplace<TComp>(handle, std::forward<Args>(args)...);
		}
		template<class TComp, typename... Args>
		inline TComp& AddOrGetComponent(EntityHandle handle, Args&&... args) noexcept {
			return m_Registry.get_or_emplace<TComp>(handle, std::forward<Args>(args)...);
		}
		template<class TComp, typename... Args>
		inline TComp& AddOrReplaceComponent(EntityHandle handle, Args&&... args) noexcept {
			return m_Registry.emplace_or_replace<TComp>(handle, std::forward<Args>(args)...);
		}
		template<class TComp>
		inline void RemoveComponent(EntityHandle handle) noexcept {
			BM_CORE_ASSERT(Has<TComp>(handle), "Entity doesn't have TComp");
			m_Registry.remove<TComp>(handle);
		}
	private:
		entt::registry m_Registry;
	};
}
