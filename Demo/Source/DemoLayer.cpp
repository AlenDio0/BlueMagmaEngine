#include "DemoLayer.hpp"
#include "GameLayer.hpp"
#include <BlueMagma/Application.hpp>
#include <BlueMagma/EventDispatcher.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

DemoLayer::DemoLayer() noexcept
	: m_Sprite(*GetAsset<BM::Texture>("Cat"))
{
	m_Sprite.setOrigin(BM::Vec2f(m_Sprite.getTexture().getSize()).Center());
	m_Sprite.setPosition(GetWindow().GetSize().Center());
	m_Sprite.setScale({ 0.4f, 0.4f });

	m_SoundManager.Add("transition", *GetAsset<BM::SoundBuffer>("Generic"));
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
		QueueTransitionTo<GameLayer>();
		break;
	case Key::B:
		GetApp().QueuePushLayer<GameLayer>();
		break;
	case Key::C:
		QueueRemoveLayer();
		break;
	default:
		return false;
	}

	return true;
}

bool DemoLayer::OnMousePressed(const BM::EventHandle::MouseButtonPressed& mousePressed) noexcept
{
	m_Sprite.setPosition((BM::Vec2f)mousePressed.position);

	return true;
}
