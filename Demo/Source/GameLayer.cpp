#include "GameLayer.hpp"
#include "DemoLayer.hpp"
#include <BlueMagma/Application.hpp>
#include <BlueMagma/Log.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <format>
#include <cmath>

GameLayer::GameLayer() noexcept
	: m_Background(GetWindow().GetSize()),
	m_Text(GetAsset<BM::Font>("Minecraft"))
{
	m_Background.setFillColor(sf::Color(0x0000FF70));

	m_SoundManager.Add("sound", GetAsset<BM::SoundBuffer>("Generic"));
}

void GameLayer::OnAttach() noexcept
{
	BM_FN();
}

void GameLayer::OnDetach() noexcept
{
	BM_FN();
}

void GameLayer::OnTransition() noexcept
{
	BM_FN();
}

void GameLayer::OnEvent(BM::Event& event) noexcept
{
	BM::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<BM::EventHandle::KeyPressed>(BM_EVENT_FN(OnKeyPressed));
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
	{
		using namespace sf::Keyboard;
		std::string key = getDescription(delocalize(keyPressed.code)).toAnsiString();
		BM_FN("keyPressed: {}", key);
	}

	switch (keyPressed.code)
	{
		using Key = sf::Keyboard::Key;

	case Key::A:
		QueueTransitionTo<DemoLayer>();
		break;
	case Key::B:
		GetApp().QueuePushLayer<DemoLayer>();
		break;
	case Key::C:
		QueueRemoveLayer();
		break;
	case Key::D:
		m_SoundManager.Play("sound");
		break;
	case Key::E:
		m_SoundManager.Play("sound", true);
		break;
	case Key::F:
		m_SoundManager.PlayThread("sound");
		break;
	case Key::S:
		m_SoundManager.Stop("sound");
		break;
	default:
		return false;
	}

	return true;
}
