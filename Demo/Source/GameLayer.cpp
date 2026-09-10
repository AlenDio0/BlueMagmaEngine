#include "GameLayer.hpp"
#include "DemoLayer.hpp"
#include "PaddleLayer.hpp"
#include <BlueMagma/Framework/Application.hpp>
#include <BlueMagma/Core/Log.hpp>
#include <BlueMagma/Math/Color.hpp>
#include <BlueMagma/Core/Random.hpp>
#include <Scene/Entity.hpp>
#include <Scene/System/TransformSystem.hpp>
#include <Scene/System/RenderSystem.hpp>
#include <Scene/System/UI/UISystem.hpp>
#include <Scene/Component/UIMaker.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Cursor.hpp>
#include <format>
#include <cmath>
#include <cfloat>
#include <cstdint>
#include <cctype>

GameLayer::GameLayer(bool initExample, bool initUIExample) noexcept
	: m_InitExample(initExample), m_InitUIExample(initUIExample),
	m_ActiveCameraPtr(&m_MainCamera), m_FPSCounter(0u), m_ButtonSpeedFactor(1.f)
{
	m_Scene.AddSystem<BM::TransformSystem>(100);
	m_Scene.AddSystem<BM::UISystem>(200);
	m_Scene.AddSystem<BM::RenderSystem>();
}

void GameLayer::OnAttachApplication() noexcept
{
	BM_FN();

	auto renderer = GetRenderer().lock();
	if (!renderer)
	{
		BM_ERROR_FN("There is no Renderer to reference");
		return;
	}

	m_Scene.AttachRenderer(renderer);

	m_MainCamera = BM::Camera2D(renderer->GetCamera());
	m_ButtonCamera = BM::Camera2D(renderer->GetCamera());

	m_ButtonCamera.SetViewport({ 0.75f, 0.5f, 0.25f, 0.5f }, renderer->GetSize());

	m_MainFontPtr = &GetAsset<BM::Font>("Minecraft Nearest");
}

void GameLayer::OnAttach() noexcept
{
	BM_FN();

	namespace Comp = BM::Component;

	auto window = GetWindow().lock();
	if (!window)
	{
		BM_ERROR_FN("There is no Window to reference");
		return;
	}

	const BM::Vec2f cBackgroundSize = window->GetSize();

	m_Background = m_Scene.CreateEntity({ .State{.Position{0.f}}, .Z = -1.f });
	m_Background.Add<Comp::RectShape>(cBackgroundSize);
	m_Background.Add<Comp::ColorMaterial>(BM::ColorDef::Green.WithAlpha(0.5f));

	m_StatText = m_Scene.CreateEntity({ .State{.Position{0.f}}, .Z = 3.f });
	m_StatText.Add<Comp::TextRender>(m_MainFontPtr, FormatStatText(0.f));
	m_StatText.Add<Comp::ColorMaterial>(BM::ColorDef::White);
	m_StatText.Add<Comp::Outline>(BM::ColorDef::Black, 1.f);

	constexpr float cAxisThickness = 3.f;
	BM::Entity axisX = m_Scene.CreateEntity({ .State{.Position = cBackgroundSize.Center(), .Origin{0.5f}}, .Z = 5.f });
	axisX.Add<Comp::RectShape>(BM::Vec2f(cBackgroundSize.X, cAxisThickness));
	axisX.Add<Comp::ColorMaterial>(BM::ColorDef::White);
	BM::Entity axisY = m_Scene.CreateEntity({ .State{.Position = cBackgroundSize.Center(), .Origin{0.5f}}, .Z = 5.f });
	axisY.Add<Comp::RectShape>(BM::Vec2f(cAxisThickness, cBackgroundSize.Y));
	axisY.Add<Comp::ColorMaterial>(BM::ColorDef::White);

	m_MouseRender = m_Scene.CreateEntity({ .State{.Position = cBackgroundSize.Center(), .Origin{0.5f}}, .Z = 10.f });
	m_MouseRender.Add<Comp::CircleShape>(0.f);
	m_MouseRender.Add<Comp::ColorMaterial>(BM::ColorDef::Clear);
	m_MouseRender.Add<Comp::Outline>(BM::ColorDef::Red.WithAlpha(0.35f), 5.f);

	if (m_InitExample)
		InitExample();
	if (m_InitUIExample)
		InitUIExample();

	m_Scene.OnDestroy<Comp::Widget>().connect<&GameLayer::UpdateMouseCursor>(this);
	m_Scene.OnUpdate<Comp::Widget>().connect<&GameLayer::UpdateMouseCursor>(this);
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
	if (auto renderer = GetRenderer().lock())
		renderer->SetCamera(*m_ActiveCameraPtr);

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
	m_FPSCounter++;
	if (m_FPSTimer.AsSeconds() >= 1.f)
	{
		m_FPSTimer.Restart();

		m_StatText.Patch<BM::Component::TextRender>([&](auto& text) {
			text.Text = FormatStatText(deltaTime);
			});

		m_FPSCounter = 0u;
	}

	if (auto renderer = GetRenderer().lock())
		renderer->SetCamera(*m_ActiveCameraPtr);

	m_Scene.OnUpdate(deltaTime);

	if (m_FocusText && m_InputText)
	{
		const bool cFocus = m_InputText.Get<BM::Component::Widget>().Focus;

		m_FocusText.Patch<BM::Component::TextRender>([&](auto& text) {
			text.Text = std::format("Focus: {}", cFocus);
			});
		m_FocusText.Patch<BM::Component::ColorMaterial>([&](auto& material) {
			material.Color = cFocus ? BM::ColorDef::Green : BM::ColorDef::Red;
			});
	}

	auto window = GetWindow().lock();
	if (!window)
		return;

	const bool cAnyInputTextFocus = m_Scene.ViewAnyOf<BM::Component::Widget, BM::Component::InputText>(
		[&](auto entity, const auto& widget, const auto& input) { return widget.Focus; });
	if (window->HasFocus() && !cAnyInputTextFocus)
	{
		namespace Keyboard = sf::Keyboard;
		using Key = Keyboard::Key;

		BM::Vec2f direction{ 0.f };
		if (Keyboard::isKeyPressed(Key::W))
			direction += BM::Vec2f::Up();
		if (Keyboard::isKeyPressed(Key::S))
			direction += BM::Vec2f::Down();
		if (Keyboard::isKeyPressed(Key::A))
			direction += BM::Vec2f::Left();
		if (Keyboard::isKeyPressed(Key::D))
			direction += BM::Vec2f::Right();

		if (direction != BM::Vec2f::Zero())
		{
			const bool cLShiftKey = Keyboard::isKeyPressed(Key::LShift);

			constexpr float cSpeed = 500.f;
			const float cCameraSpeed = (cSpeed / m_MainCamera.GetZoomFactor()) * (cLShiftKey ? 5.f : 1.f) * deltaTime;
			const BM::Vec2f cCameraOffset = direction.Normalized() * cCameraSpeed;
			m_MainCamera.Move(cCameraOffset);

			UpdateMouseRender(window->GetMousePosition());
		}
	}

	if (m_Button)
	{
		m_Button.Patch<BM::Component::Transform>([&](auto& transform) {
			const BM::RectFloat cCameraBounds = m_ActiveCameraPtr->GetBounds();
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

			constexpr float cRotationSpeed = 30.f;
			float& rotation = transform.Local.State.Rotation;

			const size_t cRotationCount = (size_t)(rotation / 360.f);
			rotation += (cRotationSpeed * (float)(cRotationCount + 1ull)) * deltaTime;

			m_ButtonCamera.SetCenter(transform.Global.Position);
			});

		if (m_ActiveCameraPtr == &m_ButtonCamera)
			UpdateMouseRender(window->GetMousePosition());
	}
}

void GameLayer::OnRender() noexcept
{
	auto renderer = GetRenderer().lock();
	if (!renderer)
		return;

	renderer->SetCamera(m_MainCamera);
	m_Scene.OnRender();

	if (m_Button)
	{
		renderer->SetCamera(m_ButtonCamera);
		m_Scene.OnRender();
	}
}

void GameLayer::InitExample() noexcept
{
	auto window = GetWindow().lock();
	if (!window)
	{
		BM_ERROR_FN("There is no Window to reference size");
		return;
	}

	namespace Comp = BM::Component;

	const BM::Texture* texture = &GetAsset<BM::Texture>("Cat");
	constexpr float cBoxSize = 50.f;
	const float cBoundSize = window->GetSize().Y - cBoxSize;

	const uint32_t cSize = (uint32_t)((float)window->GetSize().X / cBoxSize);
	const float cSizeTotal = cSize - 1.f;
	for (uint32_t i = 0; i < cSize; i++)
	{
		const float cPercentage = ((float)i + FLT_EPSILON) / cSizeTotal;
		const float cPosX = cPercentage * (window->GetSize().X - cBoxSize);
		const float cBasePosY = cPercentage * cBoundSize;

		const uint8_t cColor = (uint8_t)(cPercentage * 255.f);

		BM::Entity rect = m_Scene.CreateEntity({ .State{.Position{cPosX, cBasePosY}, .Rotation = 45.f}, .Z = 0.f });
		rect.Add<Comp::RectShape>(BM::Vec2f(cBoxSize), 5.f);
		rect.Add<Comp::ColorMaterial>(BM::Color(cColor, 0, 0));
		rect.Add<Comp::Outline>(BM::ColorDef::Black, i % 2 ? 0.f : 2.f);

		BM::Entity circle = m_Scene.CreateEntity({ .State{.Position{cPosX, cPercentage * cBasePosY}}, .Z = 0.1f });
		circle.Add<Comp::CircleShape>(cBoxSize / 2.f);
		circle.Add<Comp::ColorMaterial>(BM::Color(0, cColor, 0));
		circle.Add<Comp::Outline>(BM::ColorDef::Black, i % 2 == 0 ? 0.f : 1.f);

		BM::Entity sprite = m_Scene.CreateEntity({ .State{.Position{cPosX, cBoundSize - (cPercentage * cBasePosY)},
			.Scale{BM::Vec2f(cBoxSize) / texture->getSize()}, .Rotation = -45.f}, .Z = 0.2f, });
		sprite.Add<Comp::SpriteShape>(texture);
		sprite.Add<Comp::ColorMaterial>(BM::Color(cColor, cColor, cColor));

		BM::Entity text = m_Scene.CreateEntity({ .State{.Position{cPosX, cBoundSize - cBasePosY}}, .Z = 0.3f });
		text.Add<Comp::TextRender>(m_MainFontPtr, "O", (uint32_t)cBoxSize);
		text.Add<Comp::ColorMaterial>(BM::Color(cColor, 0, cColor));
		text.Add<Comp::Outline>(BM::ColorDef::Black, 5.f);
	}
}

void GameLayer::InitUIExample() noexcept
{
	auto window = GetWindow().lock();
	if (!window)
	{
		BM_ERROR_FN("There is no Window to reference");
		return;
	}

	namespace Comp = BM::Component;

	const BM::Vec2f cWindowSize = window->GetSize();
	constexpr BM::Vec2f cUISize(200.f, 40.f);

	m_Button = BM::UIMaker::CreateButton(m_Scene,
		{ .Transform{.State{.Position = cWindowSize.Center(), .Origin{0.5f}}, .Z = 10.f}, .Size = cUISize,
		.Corner = 5.f, .Color{BM::ColorDef::Blue}, .Outline{{BM::ColorDef::Red, 2.f}} },
		[&](auto entity, auto event) {
			BM_INFO("Button pressed");
			m_Scene.Destroy(entity);
			return false;
		});
	BM::UIMaker::AddTextChild(m_Button,
		{ .Transform{.State{.Position = BM::UIMaker::Center(cUISize, BM::Vec2f(0.5f)), .Origin = BM::Vec2f(0.5f)}, .Z = 1.f},
		.Text{m_MainFontPtr, "Hello World!"}, .Color{BM::ColorDef::Green}, .Outline{{BM::ColorDef::Black, 1.f}} });
	BM::UIMaker::AddWidgetColor(m_Button, 0.5f, 1.f);

	auto text = m_Button.CreateChild({ .State{.Position{0.f, 100.f}, .Origin = BM::Vec2f(0.5f)} });
	text.Add<Comp::TextRender>(Comp::TextRender{ .FontPtr = m_MainFontPtr, .Text = "Attached" });

	auto testButton = BM::UIMaker::CreateButton(m_Scene,
		{ .Transform{.State{.Position{cWindowSize.Center().X, cWindowSize.Y - 200.f}, .Scale{3.f, 2.f}, .Origin{0.1f, 0.5f},
		.Rotation = 90.f}, .Z = 10.f}, .Size = BM::Vec2f(80.f), .Shape = Comp::Widget::ShapeType::Circle,
		.Color{BM::ColorDef::Cyan}, .Outline{{BM::ColorDef::Yellow, 1.f }} },
		[&](BM::Entity entity, auto event) {
			static size_t sPressedCount = 0;
			sPressedCount++;
			BM_INFO("Test Button pressed {} times", sPressedCount);

			entity.Patch<Comp::Transform>([&](auto& transform) { transform.Local.State.Rotation += 10.f; });

			return false;
		});
	BM::UIMaker::AddTextChild(testButton,
		{ .Transform{.State{.Position = BM::UIMaker::Center(BM::Vec2f(80.f), {0.1f, 0.5f}), .Scale{1.5f / 3.f, 1.5f / 2.f}, .Origin = BM::Vec2f(0.5f)},
		.Z = 1.f}, .Text{m_MainFontPtr, "PRESS ME"}, .Color{BM::ColorDef::White}, .Outline{{BM::ColorDef::Black, 1.f }} });

	constexpr float cSpaceAxisX = 25.f;
	constexpr float cInputX = cUISize.X + cSpaceAxisX;

	m_InputText = BM::UIMaker::CreateInputText(m_Scene,
		{ .Transform{.State{.Position{cWindowSize.Center().X - cInputX, cWindowSize.Y / 3.f}},
		.Z = 10.f}, .Size = cUISize, .Corner = 5.f, .Color{BM::ColorDef::White}, .Outline{{BM::ColorDef::Black, 1.f }} },
		{ .Transform{.State{.Position{10.f, BM::UIMaker::Center(cUISize, BM::Vec2f(0.f)).Y}, .Origin{0.f, 0.5f}}, .Z = 1.f },
		.Text{ m_MainFontPtr }, .Color{BM::ColorDef::Black} }, { .Placeholder = "hello" });
	BM::UIMaker::AddWidgetColor(m_InputText, 0.7f, 0.85f);

	m_FocusText = m_InputText.CreateChild({ .State{.Position{cInputX + cSpaceAxisX, cUISize.Center().Y}, .Origin{0.f, 0.5f}} });
	m_FocusText.Add<Comp::TextRender>(m_MainFontPtr);
	m_FocusText.Add<Comp::ColorMaterial>(BM::ColorDef::Red);

	BM::Entity pinInputText = BM::UIMaker::CreateInputText(m_Scene,
		{ .Transform{.State{.Position{cWindowSize.Center().X - cInputX, cWindowSize.Y / 3.f}, .Scale{0.9f}, .Origin{0.7f, 0.2f}},
		.Z = 1.f}, .Size = cUISize, .Corner = 5.f, .Color{BM::ColorDef::Magenta} },
		{ .Transform{.State{.Position = BM::UIMaker::Center(cUISize, {0.7f, 0.2f}), .Origin = BM::Vec2f(0.5f)}, .Z = 1.f},
		.Text{m_MainFontPtr}, .Color{BM::ColorDef::Black} }, { .Placeholder = "PIN", .Policy{isdigit} });
	BM::UIMaker::AddWidgetColor(pinInputText, 0.75f, 1.f);
}

bool GameLayer::OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept
{
	auto window = GetWindow().lock();
	if (!window)
		return false;

	switch (keyPressed.code)
	{
		using Key = sf::Keyboard::Key;

	case Key::G:
		QueueTransitionTo<GameLayer>();
		break;
	case Key::H:
		QueueRemoveLayer();
		break;

	case Key::T:
		QueueTransitionTo<DemoLayer>();
		break;
	case Key::Y:
		GetApp().QueuePushLayer<DemoLayer>();
		break;

	case Key::P:
		QueueTransitionTo<Paddle::PaddleLayer>();
		break;

	case Key::Z:
		if (!m_InitExample)
		{
			m_InitExample = true;
			InitExample();
		}
		else
		{
			QueueTransitionTo<GameLayer>(!m_InitExample, m_InitUIExample);
		}
		break;
	case Key::X:
		if (!m_InitUIExample)
		{
			m_InitUIExample = true;
			InitUIExample();
		}
		else
		{
			QueueTransitionTo<GameLayer>(m_InitExample, !m_InitUIExample);
		}
		break;

	case Key::C:
	{
		static bool sState = false;

		std::string title = sState ? BM::WindowContext().Title : "Cat Window";
		std::filesystem::path path = sState ? BM::WindowContext().IconPath : std::filesystem::path("Asset") / "Cat.png";

		sState = !sState;

		window->SetTitle(title);
		window->SetIconFromPath(path);
	}
	break;

	case Key::B:
		m_MainCamera = BM::Camera2D(window->GetSize());
		break;
	case Key::N:
		if (auto renderer = GetRenderer().lock())
			m_MainCamera = renderer->GetDefaultCamera();
		break;

	case Key::M:
	{
		static bool sSizeSwitch = true;

		m_InputText.Patch<BM::Component::Widget>([](auto& widget) {
			widget.Size *= sSizeSwitch ? 2.f : 0.5f;
			});
		sSizeSwitch = !sSizeSwitch;
	}
	break;

	case Key::J:
		m_SoundManager.Play("sound");
		break;
	case Key::K:
		m_SoundManager.Play("sound", true);
		break;
	case Key::O:
		m_SoundManager.PlayThread("sound");
		break;
	case Key::L:
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
	auto window = GetWindow().lock();
	if (!window)
		return false;

	if (m_Button && m_ButtonCamera.Contains(mouseMoved.position, window->GetSize()))
		m_ActiveCameraPtr = &m_ButtonCamera;
	else
		m_ActiveCameraPtr = &m_MainCamera;

	if (auto renderer = GetRenderer().lock())
		renderer->SetCamera(*m_ActiveCameraPtr);

	UpdateMouseRender(mouseMoved.position);

	return false;
}

bool GameLayer::OnMousePressed(const BM::EventHandle::MouseButtonPressed& mousePressed) noexcept
{
	if (mousePressed.button != sf::Mouse::Button::Right)
		return false;

	auto renderer = GetRenderer().lock();
	if (!renderer)
		return false;

	const BM::Vec2f cMouseCoords = renderer->PixelToCoords(mousePressed.position);
	const static float cRadius = static_cast<float>(BM_RANDOM(50, 100));

	auto onCirclePressed = [&](auto entity, auto event) {
		if (event.button != sf::Mouse::Button::Left)
			return false;

		m_Scene.Destroy(entity);
		return true;
		};

	const BM::Color cRandomColor{ (static_cast<uint32_t>(BM_RANDOM(0, 0xFFFFFFFF)) << 8) | 0xFF };
	static float sPositionZ = 100.f;
	sPositionZ += 0.1f;

	BM::Entity circle = BM::UIMaker::CreateButton(m_Scene,
		{ .Transform{.State{.Position = cMouseCoords, .Origin = BM::Vec2f(0.5f)}, .Z = sPositionZ},
		.Size = BM::Vec2f(cRadius * 2.f), .Shape = BM::Component::Widget::ShapeType::Circle,
		.Color{BM::ColorDef::Clear}, .Outline{{cRandomColor, 10.f}} }, onCirclePressed);

	BM::Entity center = circle.CreateChild({ .State{.Origin = BM::Vec2f(0.5f)} });
	center.Add<BM::Component::CircleShape>(5.f);
	center.Add<BM::Component::ColorMaterial>(BM::ColorDef::Red);

	return false;
}

bool GameLayer::OnMouseScrolled(const BM::EventHandle::MouseWheelScrolled& mouseScrolled) noexcept
{
	auto renderer = GetRenderer().lock();
	if (!renderer)
		return false;

	const BM::Vec2i cMousePosition = mouseScrolled.position;
	const BM::Vec2f cMouseBeforeZoom = renderer->PixelToCoords(cMousePosition);

	const float cZoomAmount = m_MainCamera.GetZoomFactor() / 10.f;
	if (mouseScrolled.delta > 0.f)
		m_MainCamera.ZoomIn(cZoomAmount, 20.f);
	else if (mouseScrolled.delta < 0.f)
		m_MainCamera.ZoomOut(cZoomAmount, 0.5f);
	const BM::Vec2f cMouseAfterZoom = renderer->PixelToCoords(cMousePosition, m_MainCamera);

	m_MainCamera.Move(cMouseBeforeZoom - cMouseAfterZoom);

	UpdateMouseRender(cMousePosition);

	return false;
}

void GameLayer::UpdateMouseCursor() noexcept
{
	if (GetWindow().expired())
		return;

	auto windowHandle = GetWindow().lock()->GetHandle().lock();
	if (!windowHandle)
		return;

	namespace Comp = BM::Component;

	const bool cIsAnyClickableHover = m_Scene.ViewAnyOf<BM::Component::Widget, BM::Component::Clickable>(
		[](auto entity, const auto& widget, const auto& clickable) { return widget.Hover; });
	const bool cIsAnyInputTextHover = m_Scene.ViewAnyOf<BM::Component::Widget, BM::Component::InputText>(
		[](auto entity, const auto& widget, const auto& inputText) { return widget.Hover; });

	windowHandle->setMouseCursor(sf::Cursor::createFromSystem(cIsAnyClickableHover || cIsAnyInputTextHover ?
		sf::Cursor::Type::Hand : sf::Cursor::Type::Arrow).value());
}

void GameLayer::UpdateMouseRender(BM::Vec2i point) noexcept
{
	auto renderer = GetRenderer().lock();
	if (m_MouseRender && renderer)
	{
		if (!m_ActiveCameraPtr->Contains(point, renderer->GetSize()))
			return;

		const BM::Vec2f cCoords = renderer->PixelToCoords(point);
		const BM::Vec2f cPosition = m_MouseRender.Get<BM::Component::Transform>().Global.Position;

		m_MouseRender.Patch<BM::Component::CircleShape>([&](auto& circle) {
			circle.Radius = std::max(cPosition.Distance(cCoords), 7.f);
			});
	}
}

std::string GameLayer::FormatStatText(float deltaTime) const noexcept
{
#ifdef NDEBUG
	std::string_view cConfiguration = "Release";
#else
	std::string_view cConfiguration = "Debug";
#endif
	const double cMilli = deltaTime * 1e3;
	const double cMicro = std::round(deltaTime * 1e6);
	const double cFPS = std::round(1.f / deltaTime);
	const size_t cEntities = m_Scene.View<BM::Component::Transform>().size();

	return std::format("{} build\n{:.5f}ms\n{}us\n{} FPS\n{} Actual FPS\n{} Entities",
		cConfiguration, cMilli, cMicro, cFPS, m_FPSCounter, cEntities);
}
