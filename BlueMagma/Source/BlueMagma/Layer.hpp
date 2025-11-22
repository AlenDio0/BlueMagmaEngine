#pragma once
#include "EventDispatcher.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <concepts>
#include <memory>

namespace BM
{
	class Application;

	class Layer
	{
	public:
		inline Layer() noexcept = default;
		inline virtual ~Layer() noexcept = default;

		inline virtual void OnAttach() noexcept {}
		inline virtual void OnDetach() noexcept {}
		inline virtual void OnTransition() noexcept {}

		inline virtual void OnEvent(Event& event) noexcept {}
		inline virtual void OnUpdate(float deltaTime) noexcept {}
		inline virtual void OnRender(sf::RenderTarget& target) noexcept {}
	protected:
		Application& GetApp() const noexcept;

		template<std::derived_from<Layer> TLayer, typename... Args>
		inline void TransitionTo(Args&&... args) noexcept {
			QueueTransition(std::move(std::make_unique<TLayer>(std::forward<Args>(args)...)));
		}
		void RemoveLayer() noexcept;
	private:
		void QueueTransition(std::unique_ptr<Layer> toLayer) noexcept;
	};
}
