#include "GameLayer.hpp"
#include "DemoLayer.hpp"
#include <BlueMagma/Application.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <print>

void GameLayer::OnAttach() noexcept
{
	std::println("GameLayer::OnAttach");
}

void GameLayer::OnDetach() noexcept
{
	std::println("GameLayer::OnDetach");
}

void GameLayer::OnTransition() noexcept
{
	std::println("GameLayer::OnTransition");
}

void GameLayer::OnEvent(const sf::Event& event) noexcept
{
	if (auto keyPressedEvent = event.getIf<sf::Event::KeyPressed>())
	{
		switch (keyPressedEvent->code)
		{
			using Key = sf::Keyboard::Key;

		case Key::A:
			TransitionTo<DemoLayer>();
			break;
		case Key::B:
			GetApp().PushLayer<DemoLayer>();
			break;
		case Key::C:
			RemoveLayer();
			break;
		default:
			break;
		}
	}
}

void GameLayer::OnRender(sf::RenderTarget& target) noexcept
{
	sf::RectangleShape shape((sf::Vector2f)target.getSize());
	shape.setFillColor(sf::Color(0x0000FF80));

	target.draw(shape);
}
