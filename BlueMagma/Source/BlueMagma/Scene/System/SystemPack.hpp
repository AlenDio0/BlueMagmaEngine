#pragma once
#include "ISystem.hpp"

namespace BM
{
	template<std::derived_from<ISystem>... TSystems>
	class SystemPack : public ISystem
	{
	public:
		inline SystemPack() noexcept
			: m_Systems{ std::make_shared<TSystems>()... } {
		}

		inline virtual void OnAttach(Scene& scene) noexcept {
			for (auto& system : m_Systems)
				system->OnAttach(scene);
		}

		inline virtual void OnEvent(Scene& scene, Event& event) noexcept {
			for (auto& system : m_Systems)
				system->OnEvent(scene, event);
		}
		inline virtual void OnTick(Scene& scene, float timeStep) noexcept {
			for (auto& system : m_Systems)
				system->OnTick(scene, timeStep);
		}
		inline virtual void OnUpdate(Scene& scene, float deltaTime) noexcept {
			for (auto& system : m_Systems)
				system->OnUpdate(scene, deltaTime);
		}
		inline virtual void OnRender(Scene& scene) const noexcept {
			for (auto& system : m_Systems)
				system->OnRender(scene);
		}
	private:
		std::array<std::shared_ptr<ISystem>, sizeof...(TSystems)> m_Systems;
	};
}
