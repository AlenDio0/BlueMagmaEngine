#include "GameLayer.hpp"
#include "DemoLayer.hpp"
#include <BlueMagma/Base/Application.hpp>
#include <BlueMagma/Core/Log.hpp>
#include <BlueMagma/Core/Random.hpp>
#include <Scene/Entity.hpp>
#include <Scene/System/TransformSystem.hpp>
#include <Scene/System/RenderSystem.hpp>
#include <Scene/System/UISystem.hpp>
#include <Scene/Component/UIMaker.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Cursor.hpp>
#include <format>
#include <cmath>
#include <cfloat>
#include <cstdint>
#include <cctype>

GameLayer::GameLayer() noexcept
	: m_ActiveCameraPtr(&m_MainCamera), m_MainCamera(GetRenderer().GetCamera()), m_ButtonCamera(GetRenderer().GetCamera()),
	m_ButtonSpeedFactor(1.f)
{
	m_ButtonCamera.SetViewport({ 0.75f, 0.5f, 0.25f, 0.5f }, GetWindow().GetSize());

	m_SoundManager.Add("sound", GetAsset<BM::SoundBuffer>("Generic"));

	m_Scene.AttachRenderer(GetRenderer());

	m_Scene.AddSystem<BM::TransformSystem>();
	m_Scene.AddSystem<BM::UISystem>();
	m_Scene.AddSystem<BM::RenderSystem>();
}

void GameLayer::OnAttach() noexcept
{
	BM_FN();

	namespace Comp = BM::Component;

	auto font = &GetAsset<BM::Font>("Minecraft");
	const BM::Vec2f cWindowSize = GetWindow().GetSize();

	m_Background = m_Scene.CreateEntity({ .State{.Position{0.f}}, .Z = -1.f });
	m_Background.Add<Comp::RectRender>(cWindowSize);
	m_Background.Add<Comp::Style>(sf::Color(0x00FF00A0));

	m_StatText = m_Scene.CreateEntity({ .State{.Position{0.f}}, .Z = 3.f });
	m_StatText.Add<Comp::TextRender>(font, FormatStatText(0.f));
	m_StatText.Add<Comp::Style>(sf::Color::White, sf::Color::Black, 1.f);

	constexpr float cAxisThickness = 3.f;
	BM::Entity axisX = m_Scene.CreateEntity({ .State{.Position = cWindowSize.Center(), .Origin{0.5f}}, .Z = 5.f });
	axisX.Add<Comp::RectRender>(BM::Vec2f(cWindowSize.X, cAxisThickness));
	BM::Entity axisY = m_Scene.CreateEntity({ .State{.Position = cWindowSize.Center(), .Origin{0.5f}}, .Z = 5.f });
	axisY.Add<Comp::RectRender>(BM::Vec2f(cAxisThickness, cWindowSize.Y));

	m_MouseRect = m_Scene.CreateEntity({ .State{.Position = cWindowSize.Center(), .Origin{0.5f}}, .Z = 100.f });
	m_MouseRect.Add<Comp::RectRender>(cWindowSize.Center(), 5.f);
	m_MouseRect.Add<Comp::Style>(sf::Color::Transparent, sf::Color(0xFF000060), 5.f);

	//InitExample();
	InitUIExample();
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
	GetRenderer().SetCamera(*m_ActiveCameraPtr);

	m_Scene.OnEvent(event);

	BM::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<BM::EventHandle::Resized>(BM_EVENT_FN(m_MainCamera.OnViewportResizeEvent));
	dispatcher.Dispatch<BM::EventHandle::Resized>(BM_EVENT_FN(m_ButtonCamera.OnViewportResizeEvent));

	dispatcher.Dispatch<BM::EventHandle::KeyPressed>(BM_EVENT_FN(OnKeyPressed));
	dispatcher.Dispatch<BM::EventHandle::MouseMoved>(BM_EVENT_FN(OnMouseMoved));
	dispatcher.Dispatch<BM::EventHandle::MouseButtonPressed>(BM_EVENT_FN(OnMousePressed));
	dispatcher.Dispatch<BM::EventHandle::MouseWheelScrolled>(BM_EVENT_FN(OnMouseScrolled));
}

void GameLayer::OnUpdate(float deltaTime) noexcept
{
	GetRenderer().SetCamera(*m_ActiveCameraPtr);

	m_Scene.OnUpdate(deltaTime);

	const bool cAnyInputTextFocus = m_Scene.ViewAnyOf<BM::Component::Widget, BM::Component::InputText>(
		[&](auto entity, const auto& widget, const auto& input) { return widget.Focus; });
	if (GetWindow().HasFocus() && !cAnyInputTextFocus)
	{
		namespace Keyboard = sf::Keyboard;
		using Key = Keyboard::Key;

		BM::Vec2f direction(0.f, 0.f);
		if (Keyboard::isKeyPressed(Key::W))
			direction.Y -= 1.f;
		if (Keyboard::isKeyPressed(Key::S))
			direction.Y += 1.f;
		if (Keyboard::isKeyPressed(Key::A))
			direction.X -= 1.f;
		if (Keyboard::isKeyPressed(Key::D))
			direction.X += 1.f;

		if (direction != 0.f)
		{
			const bool cLShiftKey = Keyboard::isKeyPressed(Key::LShift);

			constexpr float cSpeed = 500.f;
			const float cCameraSpeed = (cSpeed / m_MainCamera.GetZoomFactor()) * (cLShiftKey ? 5.f : 1.f) * deltaTime;
			const BM::Vec2f cCameraOffset = direction.Normalized() * cCameraSpeed;
			m_MainCamera.Move(cCameraOffset);

			UpdateMouseRect(GetWindow().GetMousePosition());
		}
	}

	if (m_Button)
	{
		m_Button.Patch<BM::Component::Transform>([&](auto& transform) {
			const BM::RectFloat cCameraBounds = m_MainCamera.GetBounds();
			const float cCameraLeft = cCameraBounds.Min().X;
			const float cCameraRight = cCameraBounds.Max().X;

			const float cWidth = m_Button.Get<BM::Component::Widget>().Size.X * transform.Global.Scale.X;
			const float cLeft = cWidth * transform.Local.State.Origin.X;
			const float cRight = cWidth * (1.f - transform.Local.State.Origin.X);

			constexpr float cSpeed = 300.f;
			float& positionX = transform.Local.State.Position.X;
			positionX += cSpeed * m_ButtonSpeedFactor * deltaTime;

			if (positionX - cLeft >= cCameraRight)
			{
				positionX = cCameraLeft - cRight;
				m_ButtonSpeedFactor += 0.1f;
			}

			m_ButtonCamera.SetCenter(transform.Global.Position);
			});

		if (m_ActiveCameraPtr == &m_ButtonCamera)
			UpdateMouseRect(GetWindow().GetMousePosition());
	}

	if (m_FocusText && m_InputText)
	{
		bool focus = m_InputText.Get<BM::Component::Widget>().Focus;

		m_FocusText.Patch<BM::Component::TextRender>([&](auto& text) {
			text.Text = std::format("Focus: {}", focus);
			});
		m_FocusText.Patch<BM::Component::Style>([&](auto& style) {
			style.FillColor = focus ? sf::Color::Green : sf::Color::Red;
			});
	}

	if (m_StatText && m_StatTimer.AsSeconds() >= 0.5f)
	{
		m_StatTimer.Restart();
		m_StatText.Patch<BM::Component::TextRender>([&](auto& text) {
			text.Text = FormatStatText(deltaTime);
			});
	}
}

void GameLayer::OnRender() noexcept
{
	GetRenderer().SetCamera(m_MainCamera);
	m_Scene.OnRender();

	if (m_Button)
	{
		GetRenderer().SetCamera(m_ButtonCamera);
		m_Scene.OnRender();
	}
}

void GameLayer::InitExample() noexcept
{
	namespace Comp = BM::Component;

	const BM::Texture& texture = GetAsset<BM::Texture>("Cat");
	constexpr float cBoxSize = 25.f;
	const float cBoundSize = GetWindow().GetSize().Y - cBoxSize;

	const uint32_t cSize = (uint32_t)((float)GetWindow().GetSize().X / cBoxSize);
	const float cSizeTotal = cSize - 1.f;
	for (uint32_t i = 0; i < cSize; i++)
	{
		const float cPercentage = ((float)i + FLT_EPSILON) / cSizeTotal;
		const float cPosX = cPercentage * (GetWindow().GetSize().X - cBoxSize);
		const float cBasePosY = cPercentage * cBoundSize;

		const uint8_t cColor = (uint8_t)(cPercentage * 255.f);

		BM::Entity rect = m_Scene.CreateEntity({ .State{.Position{cPosX, cBasePosY}}, .Z = cPercentage });
		rect.Add<Comp::RectRender>(cBoxSize, 5.f);
		rect.Add<Comp::Style>(sf::Color(cColor, 0, 0), sf::Color::Black, i % 2 ? 0.f : 2.f);

		BM::Entity circle = m_Scene.CreateEntity({ .State{.Position{cPosX, cPercentage * cBasePosY}},.Z = 0.1f + cPercentage });
		circle.Add<Comp::CircleRender>(cBoxSize / 2.f);
		circle.Add<Comp::Style>(sf::Color(0, cColor, 0), sf::Color::Black, i % 2 == 0 ? 0.f : 1.f);

		BM::Entity sprite = m_Scene.CreateEntity({ .State{.Position{cPosX, cBoundSize - (cPercentage * cBasePosY)},
			.Scale{BM::Vec2f(cBoxSize) / texture.getSize()}}, .Z = 0.2f + cPercentage, });
		sprite.Add<Comp::TextureRender>(&texture);
		sprite.Add<Comp::Style>(sf::Color(cColor, cColor, cColor));

		BM::Entity text = m_Scene.CreateEntity({ .State{.Position{cPosX, cBoundSize - cBasePosY}}, .Z = 0.3f + cPercentage });
		text.Add<Comp::TextRender>(&GetAsset<BM::Font>("Minecraft"), "O", (uint32_t)cBoxSize);
		text.Add<Comp::Style>(sf::Color(cColor, 0, cColor), sf::Color::Black, 5.f);
	}
}

void GameLayer::InitUIExample() noexcept
{
	namespace Comp = BM::Component;

	auto font = &GetAsset<BM::Font>("Minecraft");
	const BM::Vec2f cWindowSize = GetWindow().GetSize();
	constexpr BM::Vec2f cUISize(200.f, 40.f);

	m_Button = BM::UIMaker::CreateButton(m_Scene, { .Transform{.State{.Position = cWindowSize.Center(), .Origin{0.5f}}, .Z = 10.f},
		.Size = cUISize, .Corner = 5.f, .Style{sf::Color::Blue, sf::Color::Red, 2.f} },
		[&](auto entity, auto event) {
			BM_INFO("Button pressed");
			m_Scene.Destroy(entity);
			return false;
		});
	BM::UIMaker::AddTextChild(m_Button, { .Transform{.State{.Position = BM::UIMaker::Center(cUISize, 0.5f), .Origin{0.5f}},
		.Z = 1.f}, .Text{font, "Hello World!"}, .Style{sf::Color::Green, sf::Color::Black, 1.f} });
	BM::UIMaker::AddHoverColor(m_Button, 0.5f);

	auto text = m_Button.CreateChild({ .State{.Position{0.f, 100.f}, .Origin{0.5f}} });
	text.Add<Comp::TextRender>(Comp::TextRender{ .FontPtr = font, .Text = "Attached" });

	constexpr float cSpaceAxisX = 25.f;
	constexpr float cInputX = cUISize.X + cSpaceAxisX;

	m_InputText = BM::UIMaker::CreateInputText(m_Scene,
		{ .Transform{.State{.Position{cWindowSize.Center().X - cInputX, cWindowSize.Y / 3.f}}, .Z = 10.f},
		.Size = cUISize, .Corner = 5.f, .Style{sf::Color::White, sf::Color::Black, 1.f} },
		{ .Transform{.State{.Position{5.f, BM::UIMaker::Center(cUISize, 0.f).Y}, .Origin{0.f, 0.5f}}, .Z = 1.f },
		.Text{ font }, .Style{ sf::Color::Black }
		}, { .Placeholder = "hello" });
	BM::UIMaker::AddHoverColor(m_InputText, 0.9f);

	m_FocusText = m_InputText.CreateChild({ .State{.Position{cInputX + cSpaceAxisX, cUISize.Center().Y}, .Origin{0.f, 0.5f}} });
	m_FocusText.Add<Comp::TextRender>(font);
	m_FocusText.Add<Comp::Style>(sf::Color::Red);

	BM::Entity pinInputText = BM::UIMaker::CreateInputText(m_Scene,
		{ .Transform{.State{.Position{cWindowSize.X - 50.f, 50.f}, .Scale{0.75f}, .Origin{1.f, 0.f}}, .Z = 5.f},
		.Size = cUISize, .Corner = 5.f, .Style{sf::Color::Magenta} },
		{ .Transform{.State{.Position = BM::UIMaker::Center(cUISize, {1.f, 0.f}), .Origin{0.5f}}, .Z = 1.f},
		.Text{font}, .Style{sf::Color::Black} }, { .Placeholder = "PIN", .Policy{isdigit} });
	BM::UIMaker::AddHoverColor(pinInputText);
}

bool GameLayer::OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept
{
	switch (keyPressed.code)
	{
		using Key = sf::Keyboard::Key;

	case Key::G:
		QueueTransitionTo<GameLayer>();
		break;
	case Key::T:
		QueueTransitionTo<DemoLayer>();
		break;
	case Key::Y:
		GetApp().QueuePushLayer<DemoLayer>();
		break;
	case Key::H:
		QueueRemoveLayer();
		break;

	case Key::B:
		m_MainCamera = BM::Camera2D(GetWindow().GetSize());
		break;
	case Key::N:
		m_MainCamera = GetRenderer().GetDefaultCamera();
		break;

	case Key::P:
		m_SoundManager.Play("sound");
		break;
	case Key::L:
		m_SoundManager.Play("sound", true);
		break;
	case Key::O:
		m_SoundManager.PlayThread("sound");
		break;
	case Key::K:
		m_SoundManager.Stop("sound");
		break;
	case Key::Up:
		m_SoundManager.Get("sound")->setVolume(150.f);
		break;
	case Key::Down:
		m_SoundManager.Get("sound")->setVolume(50.f);
		break;

	default:
		return false;
	}

	return true;
}

bool GameLayer::OnMouseMoved(const BM::EventHandle::MouseMoved& mouseMoved) noexcept
{
	if (m_Button && m_ButtonCamera.Contains(mouseMoved.position, GetWindow().GetSize()))
		m_ActiveCameraPtr = &m_ButtonCamera;
	else
		m_ActiveCameraPtr = &m_MainCamera;

	const bool cIsAnyClickableHover = m_Scene.ViewAnyOf<BM::Component::Widget, BM::Component::Clickable>(
		[](auto entity, const auto& widget, const auto& clickable) { return widget.Hover; });
	GetWindow().GetHandle().setMouseCursor(sf::Cursor::createFromSystem(
		cIsAnyClickableHover ? sf::Cursor::Type::Hand : sf::Cursor::Type::Arrow).value());

	GetRenderer().SetCamera(*m_ActiveCameraPtr);
	UpdateMouseRect(mouseMoved.position);

	return false;
}

bool GameLayer::OnMousePressed(const BM::EventHandle::MouseButtonPressed& mousePressed) noexcept
{
	if (mousePressed.button != sf::Mouse::Button::Right)
		return false;

	const BM::Vec2f cMouseCoords = GetRenderer().PixelToCoords(mousePressed.position);
	const float cRadius = static_cast<float>(BM_RANDOM(10, 30)) * 5.f;
	using ShapeType = BM::Component::Widget::ShapeType;

	auto onCirclePressed = [&](auto entity, auto event) {
		if (event.button != sf::Mouse::Button::Left)
			return false;

		m_Scene.Destroy(entity);
		return true;
		};

	const sf::Color cRandomColor{ (static_cast<uint32_t>(BM_RANDOM()) << 8) | 0xFF };
	BM::Entity circle = BM::UIMaker::CreateButton(m_Scene,
		{ .Transform{.State{.Position = cMouseCoords, .Origin{0.5f}}, .Z = 100.f},
		.Size{cRadius * 2.f}, .Shape{ShapeType::Circle},
		.Style{sf::Color::Transparent, cRandomColor, 10.f} }, onCirclePressed);

	BM::Entity center = circle.CreateChild({ .State{.Origin{0.5f}} });
	center.Add<BM::Component::CircleRender>(5.f);
	center.Add<BM::Component::Style>(sf::Color::Red);

	return false;
}

bool GameLayer::OnMouseScrolled(const BM::EventHandle::MouseWheelScrolled& mouseScrolled) noexcept
{
	const BM::Vec2i cMousePosition = mouseScrolled.position;
	const BM::Vec2f cMouseBeforeZoom = GetRenderer().PixelToCoords(cMousePosition);

	const float cZoomAmount = m_MainCamera.GetZoomFactor() / 10.f;
	if (mouseScrolled.delta > 0.f)
		m_MainCamera.ZoomIn(cZoomAmount, 20.f);
	else if (mouseScrolled.delta < 0.f)
		m_MainCamera.ZoomOut(cZoomAmount, 0.5f);
	const BM::Vec2f cMouseAfterZoom = GetRenderer().PixelToCoords(cMousePosition, m_MainCamera);

	m_MainCamera.Move(cMouseBeforeZoom - cMouseAfterZoom);

	UpdateMouseRect(cMousePosition);

	return false;
}

void GameLayer::UpdateMouseRect(BM::Vec2i point) noexcept
{
	if (m_MouseRect)
	{
		if (!m_ActiveCameraPtr->Contains(point, GetWindow().GetSize()))
			return;

		m_MouseRect.Patch<BM::Component::RectRender>([&](auto& rect) {
			const BM::Vec2f cCoords = GetRenderer().PixelToCoords(point);
			const BM::Vec2f cPosition = m_MouseRect.Get<BM::Component::Transform>().Global.Position;

			rect.Size = (cCoords - cPosition).Absolute() * 2.f;
			});
	}
}

std::string GameLayer::FormatStatText(float deltaTime) const noexcept
{
	const size_t cEntities = m_Scene.View<BM::Component::Transform>().size();
	const double cMicro = std::round(deltaTime * std::pow(10, 6));
	const double cFPS = std::round(1.f / deltaTime);

	return std::format("{} Entities\n{:.5f}ms\n{}us\n{} FPS", cEntities, deltaTime, cMicro, cFPS);
}
