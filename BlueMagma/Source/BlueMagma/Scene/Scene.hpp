#pragma once
#include "Core/Assert.hpp"
#include "System/ISystem.hpp"
#include "EntityHandle.hpp"
#include "Component.hpp"
#include "Base/EventDispatcher.hpp"
#include <entt/entt.hpp>
#include <entt/entity/fwd.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>
#include <concepts>
#include <memory>

namespace BM
{
	class Entity;
	class Scene
	{
	public:
		inline Scene() noexcept = default;

		entt::registry& GetRegistry() noexcept;

		template<std::derived_from<ISystem> TSystem>
		inline void AddSystem() noexcept {
			auto system = std::make_shared<TSystem>();

			system->OnAttach(*this);

			AddSystemEvent([system](Scene& scene, Event& event) { system->OnEvent(scene, event); });
			AddSystemUpdate([system](Scene& scene, float deltaTime) { system->OnUpdate(scene, deltaTime); });
			AddSystemRender([system](const Scene& scene, sf::RenderTarget& target) { system->OnRender(scene, target); });
		}

		void AddSystemEvent(SystemEventFn onEvent) noexcept;
		void AddSystemUpdate(SystemUpdateFn onUpdate) noexcept;
		void AddSystemRender(SystemRenderFn onRender) noexcept;

		void OnEvent(Event& event) noexcept;
		void OnUpdate(float deltaTime) noexcept;
		void OnRender(sf::RenderTarget& target) const noexcept;

		Entity Create(const Component::Transform& transform = {}) noexcept;
		Entity GetEntity(EntityHandle handle) noexcept;

		template<class TComp>
		inline decltype(auto) OnConstruct() noexcept {
			return m_Registry.on_construct<TComp>();
		}
		template<class TComp>
		inline decltype(auto) OnDestroy() noexcept {
			return m_Registry.on_destroy<TComp>();
		}
		template<class TComp>
		inline decltype(auto) OnUpdate() noexcept {
			return m_Registry.on_update<TComp>();
		}

		template<class... TComp>
		inline decltype(auto) View() const noexcept {
			return m_Registry.view<TComp...>();
		}

		template<class TComp>
		inline bool HasCtxComponent() const noexcept {
			return m_Registry.ctx().find<TComp>();
		}
		template<class TComp>
		inline TComp* TryGetCtxComponent() noexcept {
			return m_Registry.ctx().find<TComp>();
		}
		template<class TComp>
		inline TComp& GetCtxComponent() noexcept {
			BM_CORE_ASSERT(HasCtxComponent<TComp>(), "Context doesn't have TComp");
			return m_Registry.ctx().get<TComp>();
		}
		template<class TComp, typename... Args>
		inline TComp& AddCtxComponent(Args&&... args) noexcept {
			BM_CORE_ASSERT(!HasCtxComponent<TComp>(), "Context already has TComp");
			return m_Registry.ctx().emplace<TComp>(std::forward<Args>(args)...);
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
		entt::registry m_Registry;

		std::vector<SystemEventFn> m_SystemEvents;
		std::vector<SystemUpdateFn> m_SystemUpdates;
		std::vector<SystemRenderFn> m_SystemRenders;
	};
}
