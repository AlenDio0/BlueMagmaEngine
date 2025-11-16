#include "DemoLayer.hpp"
#include "GameLayer.hpp"
#include <BlueMagma/Application.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

void DemoLayer::OnEvent(const sf::Event& event) noexcept
{
	if (auto keyPressedEvent = event.getIf<sf::Event::KeyPressed>())
	{
		switch (keyPressedEvent->code)
		{
			using Key = sf::Keyboard::Key;

		case Key::A:
			TransitionTo<GameLayer>();
			break;
		case Key::B:
			GetApp().PushLayer<GameLayer>();
			break;
		case Key::C:
			RemoveLayer();
			break;
		default:
			break;
		}
	}
}

void DemoLayer::OnRender(sf::RenderTarget& target) noexcept
{
	sf::RectangleShape shape((sf::Vector2f)target.getSize() / 4.f);
	shape.setOrigin((sf::Vector2f)shape.getSize() / 2.f);
	shape.setPosition((sf::Vector2f)target.getSize() / 2.f);
	shape.setFillColor(sf::Color::Red);

	target.draw(shape);
}
