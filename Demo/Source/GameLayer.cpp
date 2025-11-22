#include "GameLayer.hpp"
#include "DemoLayer.hpp"
#include <BlueMagma/Application.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <print>
#include <format>
#include <cmath>

GameLayer::GameLayer() noexcept
	: m_Background((sf::Vector2f)GetApp().GetWindow().GetHandle().getSize()),
	m_Text(*GetApp().GetAssets().Get<BM::Font>("minecraft")),
	m_Sound(*GetApp().GetAssets().Get<BM::SoundBuffer>("transition"))
{
	m_Background.setFillColor(sf::Color(0x0000FF80));
	m_Sound.setVolume(10.f);
}

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

void GameLayer::OnEvent(BM::Event& event) noexcept
{
	BM::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<BM::EventHandle::KeyPressed>([&](const auto& event) { return OnKeyPressed(event); });
}

void GameLayer::OnUpdate(float deltaTime) noexcept
{
	if (m_UpdateText.getElapsedTime().asSeconds() >= 1.f)
	{
		m_UpdateText.restart();

		const double cMicro = std::round(deltaTime * std::pow(10, 6));
		const double cFPS = std::round(1.f / deltaTime);

		m_Text.setString(std::format("{}ms\n{}us\n{} FPS", deltaTime, cMicro, cFPS));
	}
}

void GameLayer::OnRender(sf::RenderTarget& target) noexcept
{
	target.draw(m_Background);
	target.draw(m_Text);
}

bool GameLayer::OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept
{
	switch (keyPressed.code)
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
	case Key::D:
		m_Sound.play();
		break;
	default:
		return false;
	}

	return true;
}
