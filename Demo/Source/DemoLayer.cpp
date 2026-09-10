#include "DemoLayer.hpp"
#include "GameLayer.hpp"
#include <BlueMagma/Event/EventDispatcher.hpp>
#include <BlueMagma/Framework/Application.hpp>

DemoLayer::DemoLayer() noexcept
	: m_Sprite(BM::Texture::GetDefault())
{
}

void DemoLayer::OnAttachApplication() noexcept
{
	m_Sprite.setTexture(GetAsset<BM::Texture>("Cat"), true);
	m_Sprite.setOrigin(BM::Vec2f(m_Sprite.getTexture().getSize()).Center());

	if (auto window = GetWindow().lock())
		m_Sprite.setPosition(window->GetSize().Center());

	m_Sprite.setScale(BM::Vec2f(300.f) / m_Sprite.getTexture().getSize());

	m_SoundManager.Add("transition", GetAsset<BM::SoundBuffer>("Generic"));
	m_SoundManager.PlayThread("transition");
}

void DemoLayer::OnEvent(BM::Event& event) noexcept
{
	BM::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<BM::EventHandle::KeyPressed>(BM_EVENT_FN(OnKeyPressed));
	dispatcher.Dispatch<BM::EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressed));
}

void DemoLayer::OnRender() noexcept
{
	auto renderer = GetRenderer().lock();
	if (!renderer)
		return;

	renderer->ResetCamera();
	renderer->Draw(m_Sprite);
}

bool DemoLayer::OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept
{
	switch (keyPressed.code)
	{
		using Key = sf::Keyboard::Key;

	case Key::Z:
		QueueTransitionTo<GameLayer>();
		break;
	case Key::X:
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
	auto renderer = GetRenderer().lock();
	if (!renderer)
		return false;

	renderer->ResetCamera();
	m_Sprite.setPosition(renderer->PixelToCoords(mousePressed.position));

	return true;
}
