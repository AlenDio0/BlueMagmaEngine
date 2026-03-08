#pragma once
#include "EntityHandle.hpp"
#include "Scene.hpp"
#include <vector>

namespace BM
{
	struct Parent { EntityHandle Handle; };
	struct Children { std::vector<EntityHandle> Handles; };

	class Entity
	{
	public:
		inline Entity() noexcept = default;
		inline Entity(const Entity&) noexcept = default;
		Entity(Scene* scene, EntityHandle handle) noexcept;
		inline ~Entity() noexcept = default;

		EntityHandle GetHandle() const noexcept;
		bool IsValid() const noexcept;

		operator EntityHandle() const noexcept;
		operator bool() const noexcept;

		Entity CreateChild(const Component::Transform& transform = {}) noexcept;

		std::vector<Entity> GetChildren() noexcept;
		template<class... TComp>
		inline std::vector<Entity> GetChildren() noexcept {
			std::vector<Entity> children;

			auto childList = GetChildren();
			for (auto& child : childList)
			{
				if (child.Has<TComp...>())
					children.push_back(child);
			}

			return children;
		}

		template<class... TComp>
		inline bool Has() const noexcept {
			return m_ScenePtr->HasComponent<TComp...>(m_Handle);
		}
		template<class TComp>
		inline decltype(auto) Get() const noexcept {
			return m_ScenePtr->GetComponent<TComp>(m_Handle);
		}
		template<class TComp, typename... TArgs>
		inline decltype(auto) Add(TArgs&&... args) noexcept {
			return m_ScenePtr->AddComponent<TComp>(m_Handle, std::forward<TArgs>(args)...);
		}
		template<class TComp, typename... TArgs>
		inline decltype(auto) Replace(TArgs&&... args) noexcept {
			return m_ScenePtr->ReplaceComponent<TComp>(m_Handle, std::forward<TArgs>(args)...);
		}
		template<class TComp, class... Funcs>
		inline decltype(auto) TryPatch(Funcs&&... funcs) noexcept {
			if (Has<TComp>())
				Patch<TComp>(std::forward<Funcs>(funcs)...);
		}
		template<class TComp, class... Funcs>
		inline decltype(auto) Patch(Funcs&&... funcs) noexcept {
			return m_ScenePtr->PatchComponent<TComp>(m_Handle, std::forward<Funcs>(funcs)...);
		}
		template<class TComp>
		inline void Remove() noexcept {
			m_ScenePtr->RemoveComponent<TComp>(m_Handle);
		}

		template<class TComp>
		inline decltype(auto) TryGet() const noexcept {
			return m_ScenePtr->TryGetComponent<TComp>(m_Handle);
		}
		template<class TComp, typename... TArgs>
		inline decltype(auto) AddOrGet(TArgs&&... args) noexcept {
			return m_ScenePtr->AddOrGetComponent<TComp>(m_Handle, std::forward<TArgs>(args)...);
		}
		template<class TComp, typename... TArgs>
		inline decltype(auto) AddOrReplace(TArgs&&... args) noexcept {
			return m_ScenePtr->AddOrReplaceComponent<TComp>(m_Handle, std::forward<TArgs>(args)...);
		}
	private:
		Scene* m_ScenePtr = nullptr;
		EntityHandle m_Handle{};
	};
}

namespace std
{
	template<>
	struct formatter<BM::Entity>
	{
		constexpr auto parse(auto& context) {
			return context.begin();
		}

		inline auto format(const BM::Entity& entity, auto& context) const noexcept {
			return std::format_to(context.out(), "{}", entity.GetHandle());
		}
	};
}
