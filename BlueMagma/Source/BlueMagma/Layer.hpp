#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <concepts>
#include <memory>

namespace BM
{
	class Application;

	class Layer
	{
	public:
		Layer() noexcept = default;
		virtual ~Layer() noexcept = default;

		virtual void OnAttach() noexcept {}
		virtual void OnDetach() noexcept {}
		virtual void OnTransition() noexcept {}

		virtual void OnEvent(const sf::Event& event) noexcept {}
		virtual void OnUpdate(float deltaTime)	noexcept {}
		virtual void OnRender(sf::RenderTarget& target) noexcept {}
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
