#include "GameLayer.hpp"
#include "DemoLayer.hpp"
#include <BlueMagma/Base/Application.hpp>
#include <BlueMagma/Core/Log.hpp>
#include <Scene/Entity.hpp>
#include <Scene/System/TransformSystem.hpp>
#include <Scene/System/RenderSystem.hpp>
#include <Scene/System/UISystem.hpp>
#include <Scene/Component/UIMaker.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <format>
#include <cmath>
#include <cfloat>
#include <cstdint>

GameLayer::GameLayer() noexcept
{
	m_SoundManager.Add("sound", GetAsset<BM::SoundBuffer>("Generic"));
}

void GameLayer::OnAttach() noexcept
{
	BM_FN();

	m_Scene.AddSystem<BM::TransformSystem>();
	m_Scene.AddSystem<BM::UISystem>();
	m_Scene.AddSystem<BM::RenderSystem>();

	using namespace BM::Component;
	using Color = sf::Color;

	auto font = &GetAsset<BM::Font>("Minecraft");
	const BM::Vec2f cWindowSize = GetWindow().GetSize();

	m_Background = m_Scene.Create(Transform(BM::Vec2f::Zero(), -1.f));
	m_Background.Add<RectRender>(cWindowSize);
	m_Background.Add<Style>(Color(0x00FF00A0));

	m_StatText = m_Scene.Create(Transform(BM::Vec2f::Zero(), 3.f));
	m_StatText.Add<TextRender>(font, FormatStatText(0.1f));
	m_StatText.Add<Style>(Color::White, Color::Black, 1.f);

	auto onButtonPressed = [](auto entity, auto event) {
		BM_INFO("Button pressed");
		return false;
		};

	const BM::Vec2f cUISize(200.f, 40.f);

	m_Button = BM::UIMaker::CreateButton(m_Scene, Transform(cWindowSize.Center(), 10.f, 1.f, 0.5f), cUISize,
		5.f, Style(Color::Blue, Color::Red, 2.f), onButtonPressed);
	BM::UIMaker::AddTextChild(m_Button, TextRender(font, "Hello World!"), Style(Color::Green, Color::Black, 1.f));
	BM::UIMaker::AddHoverColor(m_Button);

	const float cSpaceAxisX = 25.f;
	const float cInputX = cUISize._X + cSpaceAxisX;

	m_InputText = BM::UIMaker::CreateInputText(m_Scene, Transform(BM::Vec2f(cWindowSize.Center()._X - cInputX, cWindowSize._Y / 3.f), 10.f), cUISize,
		5.f, Style(Color::White, Color::Black, 1.f), TextRender(font), Style(Color::Black), InputText("hello"));
	BM::UIMaker::AddHoverColor(m_InputText, 0.9f);

	for (int i = 0; i < 100; i++)
		BM::UIMaker::CreateInputText(m_Scene, {}, {}, 0.f, {}, {}, {}, {});

	m_FocusText = m_InputText.CreateChild(Transform(BM::Vec2f(cInputX + cSpaceAxisX, cUISize.Center()._Y), 0.f, 1.f, BM::Vec2f(0.f, 0.5f)));
	m_FocusText.Add<TextRender>(font);
	m_FocusText.Add<Style>(Color::Red);

	BM::Entity axis = m_Scene.Create(Transform(BM::Vec2f(cWindowSize.Center()._X, 0.f), 100.f));
	axis.Add<RectRender>(BM::Vec2f(1.f, cWindowSize._Y));

	m_MouseRect = m_Scene.Create(Transform(cWindowSize.Center(), 100.f, 1.f, 0.5f));
	m_MouseRect.Add<RectRender>(0.f, 5.f);
	m_MouseRect.Add<Style>(Color::Transparent, Color(0xFF000080), 2.f);

	//InitExample();
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
	dispatcher.Dispatch<BM::EventHandle::MouseMoved>(BM_EVENT_FN(OnMouseMoved));
}

void GameLayer::OnUpdate(float deltaTime) noexcept
{
	m_Scene.OnUpdate(deltaTime);

	m_Button.Patch<BM::Component::Transform>([&](auto& transform) {
		static float sSpeedFactor = 1.f;

		const float cBoundsX = (float)GetWindow().GetSize()._X;
		const float cSizeX = m_Button.Get<BM::Component::Widget>()._Size._X;
		const float cPivotX = cSizeX * transform._Scale._X * transform._Origin._X;

		float& positionX = transform._LocalPosition._X;
		positionX += 500.f * sSpeedFactor * deltaTime;

		if (positionX - cPivotX >= cBoundsX)
		{
			positionX = -cPivotX;
			sSpeedFactor += 0.1f;
		}
		});

	m_FocusText.Patch<BM::Component::TextRender>([&](auto& text) {
		bool focus = m_InputText.Get<BM::Component::Widget>()._Focus;
		text._Content = std::format("Focus: {}", focus);
		});
	m_FocusText.Patch<BM::Component::Style>([&](auto& style) {
		bool focus = m_InputText.Get<BM::Component::Widget>()._Focus;
		style._FillColor = focus ? sf::Color::Green : sf::Color::Red;
		});

	if (m_SwitchTimer.AsSeconds() >= 1.f)
	{
		m_SwitchTimer.Restart();
		m_Background.Patch<BM::Component::Transform>([](auto& transform) {
			float& positionZ = transform._LocalZ;
			positionZ = positionZ == -1.f ? 2.f : -1.f;
			});
	}
	if (m_StatTimer.AsSeconds() >= 0.5f)
	{
		m_StatTimer.Restart();
		m_StatText.Patch<BM::Component::TextRender>([&](auto& text) {
			text._Content = FormatStatText(deltaTime);
			});
	}
}

void GameLayer::OnRender(sf::RenderTarget& target) noexcept
{
	m_Scene.OnRender(target);
}

void GameLayer::InitExample() noexcept
{
	using namespace BM::Component;

	const BM::Texture& texture = GetAsset<BM::Texture>("Cat");
	const float cBoxSize = 25.f;
	const float cBoundSize = GetWindow().GetSize()._Y - cBoxSize;

	const uint32_t cSize = (uint32_t)((float)GetWindow().GetSize()._X / cBoxSize);
	const float cSizeTotal = cSize - 1.f;
	for (uint32_t i = 0; i < cSize; i++)
	{
		const float cPercentage = ((float)i + FLT_EPSILON) / cSizeTotal;
		const float cPosX = cPercentage * (GetWindow().GetSize()._X - cBoxSize);
		const float cBasePosY = cPercentage * cBoundSize;

		const uint8_t cColor = (uint8_t)(cPercentage * 255.f);

		BM::Entity rect = m_Scene.Create(Transform(BM::Vec2f(cPosX, cBasePosY), cPercentage));
		rect.Add<RectRender>(cBoxSize, 5.f);
		rect.Add<Style>(sf::Color(cColor, 0, 0), sf::Color::Black, 2.f);

		BM::Entity circle = m_Scene.Create(Transform(BM::Vec2f(cPosX, cPercentage * cBasePosY), 0.1f + cPercentage));
		circle.Add<CircleRender>(cBoxSize / 2.f);
		circle.Add<Style>(sf::Color(0, cColor, 0), sf::Color::Black, i % 2 ? 0.f : 1.f);

		BM::Entity sprite = m_Scene.Create(Transform(BM::Vec2f(cPosX, cBoundSize - (cPercentage * cBasePosY)), 0.2f + cPercentage,
			BM::Vec2f(cBoxSize) / texture.getSize()));
		sprite.Add<TextureRender>(&texture);
		sprite.Add<Style>(sf::Color(cColor, cColor, cColor));

		BM::Entity text = m_Scene.Create(Transform(BM::Vec2f(cPosX, cBoundSize - cBasePosY), 0.3f + cPercentage));
		text.Add<TextRender>(&GetAsset<BM::Font>("Minecraft"), "O", (uint32_t)cBoxSize);
		text.Add<Style>(sf::Color(cColor, 0, cColor), sf::Color::Black, 5.f);
	}
}

bool GameLayer::OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept
{
	return false;

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

bool GameLayer::OnMouseMoved(const BM::EventHandle::MouseMoved& mouseMoved) noexcept
{
	m_MouseRect.Patch<BM::Component::RectRender>([&](auto& rect) {
		rect._Size = (BM::Vec2i(mouseMoved.position) - GetWindow().GetSize().Center()) * 2;
		auto& [x, y] = rect._Size;
		x = std::abs(x);
		y = std::abs(y);
		});

	return false;
}

std::string GameLayer::FormatStatText(float deltaTime) const noexcept
{
	const double cMicro = std::round(deltaTime * std::pow(10, 6));
	const double cFPS = std::round(1.f / deltaTime);

	return std::format("{:.5f}ms\n{}us\n{} FPS", deltaTime, cMicro, cFPS);
}
