#include "DemoLayer.hpp"
#include "GameLayer.hpp"
#include <BlueMagma/Application.hpp>
#include <BlueMagma/EventDispatcher.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

DemoLayer::DemoLayer() noexcept
	: m_Sprite(*GetApp().GetAssets().Get<BM::Texture>("cat"))
{
	m_Sprite.setOrigin((sf::Vector2f)m_Sprite.getTexture().getSize() / 2.f);
	m_Sprite.setPosition((sf::Vector2f)(GetApp().GetWindow().GetHandle().getSize() / 2u));
	m_Sprite.setScale({ 0.4f, 0.4f });

	m_SoundManager.Add("transition", *GetApp().GetAssets().Get<BM::SoundBuffer>("transition"));
}

void DemoLayer::OnTransition() noexcept
{
	m_SoundManager.PlayThread("transition");
}

void DemoLayer::OnEvent(BM::Event& event) noexcept
{
	BM::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<BM::EventHandle::KeyPressed>([&](const auto& event) { return OnKeyPressed(event); });
	dispatcher.Dispatch<BM::EventHandle::MouseButtonPressed>([&](const auto& event) { return OnMousePressed(event); });
}

void DemoLayer::OnRender(sf::RenderTarget& target) noexcept
{
	target.draw(m_Sprite);
}

bool DemoLayer::OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept
{
	switch (keyPressed.code)
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
		return false;
	}

	return true;
}

bool DemoLayer::OnMousePressed(const BM::EventHandle::MouseButtonPressed& mousePressed) noexcept
{
	m_Sprite.setPosition((sf::Vector2f)mousePressed.position);

	return true;
}
