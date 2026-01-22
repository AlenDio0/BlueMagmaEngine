#include "GameLayer.hpp"
#include "DemoLayer.hpp"
#include <BlueMagma/Base/Application.hpp>
#include <BlueMagma/Core/Log.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <Scene/Entity.hpp>
#include <Scene/System/TransformSystem.hpp>
#include <Scene/System/RenderSystem.hpp>
#include <BlueMagma/Core/Random.hpp>
#include <format>
#include <cmath>
#include <cfloat>

GameLayer::GameLayer() noexcept
{
	m_SoundManager.Add("sound", GetAsset<BM::SoundBuffer>("Generic"));
}

void GameLayer::OnAttach() noexcept
{
	BM_FN();

	m_Scene.AddSystem<BM::TransformSystem>();
	m_Scene.AddSystem<BM::RenderSystem>();

	using namespace BM::Component;

	m_Background = m_Scene.Create(Transform(BM::Vec2f::Zero(), -1.f));
	m_Background.Add<RectRender>(GetWindow().GetSize(), sf::Color(0x00FF0070));

	m_StatText = m_Scene.Create(Transform(BM::Vec2f::Zero(), 3.f));
	m_StatText.Add<TextRender>(&GetAsset<BM::Font>("Minecraft"), FormatStatText(0.1f));

	const BM::Texture& texture = GetAsset<BM::Texture>("Cat");
	const float cBoxSize = 25.f;
	const float cBoundSize = GetWindow().GetSize()._Y - cBoxSize;

	const int cSize = 100;
	const float cSizeTotal = cSize - 1.f;
	for (int i = 0; i < cSize; i++)
	{
		const float cIndex = (float)i + FLT_EPSILON;
		const float cPosX = (cIndex / cSizeTotal) * (GetWindow().GetSize()._X - cBoxSize);

		const sf::Color cColor((uint8_t)BM_RANDOM(), (uint8_t)BM_RANDOM(), (uint8_t)BM_RANDOM());

		m_Background.CreateChild(Transform(BM::Vec2f(cPosX, (cIndex / cSizeTotal) * cBoundSize), 0.f))
			.Add<RectRender>(cBoxSize, cColor);

		m_Background.CreateChild(Transform(BM::Vec2f(cPosX, ((cIndex * cIndex) / (cSizeTotal * cSizeTotal)) * cBoundSize), 0.1f))
			.Add<CircleRender>(cBoxSize / 2.f, cColor);

		m_Background.CreateChild(Transform(BM::Vec2f(cPosX, cBoundSize - ((cIndex * cIndex) / (cSizeTotal * cSizeTotal)) * cBoundSize), 0.2f,
			BM::Vec2f(cBoxSize) / texture.getSize())).Add<TextureRender>(&texture, cColor);
	}
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
	m_Scene.OnEvent(event);

	BM::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<BM::EventHandle::KeyPressed>(BM_EVENT_FN(OnKeyPressed));
}

void GameLayer::OnUpdate(float deltaTime) noexcept
{
	m_Scene.OnUpdate(deltaTime);
	if (m_TextTimer.AsSeconds() >= 1.f)
	{
		m_Background.Patch<BM::Component::Transform>([](auto& transform) {
			float& newZ = transform._Z;
			newZ = newZ == -1.f ? 2.f : -1.f;
			});

		m_TextTimer.Restart();

		m_StatText.Patch<BM::Component::TextRender>([&](auto& text) {
			text._Content = FormatStatText(deltaTime);
			});
	}
}

void GameLayer::OnRender(sf::RenderTarget& target) noexcept
{
	m_Scene.OnRender(target);
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
	case Key::H:
		m_Scene.RemoveSystem<BM::TransformSystem>();
		break;
	case Key::J:
		m_Scene.RemoveSystem<BM::RenderSystem>();
		break;
	default:
		return false;
	}

	return true;
}

std::string GameLayer::FormatStatText(float deltaTime) const noexcept
{
	const double cMicro = std::round(deltaTime * std::pow(10, 6));
	const double cFPS = std::round(1.f / deltaTime);

	return std::format("{:.5f}ms\n{}us\n{} FPS", deltaTime, cMicro, cFPS);
}
