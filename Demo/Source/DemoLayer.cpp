#include "DemoLayer.hpp"
#include "GameLayer.hpp"
#include <BlueMagma/Base/Application.hpp>
#include <BlueMagma/Base/EventDispatcher.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

DemoLayer::DemoLayer() noexcept
	: m_Sprite(GetAsset<BM::Texture>("Cat"))
{
	m_Sprite.setOrigin(BM::Vec2f(m_Sprite.getTexture().getSize()).Center());
	m_Sprite.setPosition(GetWindow().GetSize().Center());
	m_Sprite.setScale(BM::Vec2f(300.f) / m_Sprite.getTexture().getSize());

	m_SoundManager.Add("transition", GetAsset<BM::SoundBuffer>("Generic"));
}

void DemoLayer::OnTransition() noexcept
{
	m_SoundManager.PlayThread("transition");
}

void DemoLayer::OnEvent(BM::Event& event) noexcept
{
	BM::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<BM::EventHandle::KeyPressed>(BM_EVENT_FN(OnKeyPressed));
	dispatcher.Dispatch<BM::EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressed));
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
