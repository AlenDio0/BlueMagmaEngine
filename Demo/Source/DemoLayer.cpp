#include "DemoLayer.hpp"
#include "GameLayer.hpp"
#include <BlueMagma/Application.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Audio/SoundSource.hpp>

DemoLayer::DemoLayer() noexcept
	: m_Sprite(*GetApp().GetAssets().Get<BM::Texture>("cat")),
	m_Sound(*GetApp().GetAssets().Get<BM::SoundBuffer>("transition"))
{
	m_Sprite.setOrigin((sf::Vector2f)m_Sprite.getTexture().getSize() / 2.f);
	m_Sprite.setPosition((sf::Vector2f)(GetApp().GetWindow().GetHandle().getSize() / 2u));
	m_Sprite.setScale({ 0.4f, 0.4f });

	m_Sound.setVolume(15.f);
}

void DemoLayer::OnTransition() noexcept
{
	m_Sound.play();
	while (m_Sound.getStatus() == sf::SoundSource::Status::Playing);
}

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
	target.draw(m_Sprite);
}
