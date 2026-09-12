#include "GameLayer.hpp"
#include "DemoLayer.hpp"
#include "PaddleLayer.hpp"
#include <BlueMagma/Framework/Application.hpp>
#include <BlueMagma/Core/Log.hpp>
#include <BlueMagma/Math/Color.hpp>
#include <BlueMagma/Core/Random.hpp>
#include <Scene/Entity.hpp>
#include <Scene/System/Core/TransformSystem.hpp>
#include <Scene/System/Render/RenderSystem.hpp>
#include <Scene/System/UI/UISystem.hpp>
#include <Scene/Component/Builder/EntityBuilder.hpp>
#include <Scene/Component/Builder/RenderBuilder.hpp>
#include <Scene/Component/Builder/WidgetBuilder.hpp>
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

	m_MainCamera = BM::Camera2D(renderer->GetDefaultCamera());
	m_ButtonCamera = BM::Camera2D(renderer->GetDefaultCamera());

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

			rotation += (cRotationSpeed * (float)(m_ButtonRotationCount + 1ull)) * deltaTime;

			const size_t cCurrentRotationCount = (size_t)(rotation / 360.f);
			m_ButtonRotationCount += cCurrentRotationCount;
			rotation -= 360.f * cCurrentRotationCount;

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
		const float cOutlineThickness = i % 3 && i % 4 ? 0.f : 2.f;

		BM::RenderBuilder builder;
		builder.AtX(cPosX).WithOutline({ .Color = BM::ColorDef::Black, .Thickness = cOutlineThickness });

		builder.ToRect().AtY(cBasePosY).AtZ(0.1f)
			.WithColor(BM::Color(cColor, 0u, 0u))
			.WithSize(BM::Vec2f(cBoxSize)).WithCorner(5.f)
			.Build(m_Scene);

		builder.ToCircle().AtY(cPercentage * cBasePosY).AtZ(0.2f)
			.WithColor(BM::Color(0u, cColor, 0u))
			.WithRadius(cBoxSize / 2.f)
			.Build(m_Scene);

		builder.ToSprite().AtY(cBoundSize - (cPercentage * cBasePosY)).WithRotation(-45.f).AtZ(0.3f)
			.WithColor(BM::Color(cColor, cColor, cColor))
			.WithTexture(texture).WithScaleAsSizeTexture(texture, BM::Vec2f(cBoxSize))
			.Build(m_Scene);

		builder.ToText().AtY(cBoundSize - cBasePosY).AtZ(0.4F)
			.WithColor(BM::Color(cColor, 0u, cColor))
			.WithFont(m_MainFontPtr).WithText("O").WithCharSize((uint32_t)cBoxSize)
			.Build(m_Scene);
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
	constexpr BM::Vec2f cUISize(300.f, 50.f);

	{
		auto onButtonClick = [&](BM::Entity entity, auto event)
			{
				BM_INFO("Button has been pressed, destroyed");
				m_Scene.Destroy(entity);
				return false;
			};
		m_Button = BM::ButtonBuilder().At(cWindowSize.Center()).WithOrigin(BM::Vec2f(0.5f)).AtZ(10.f)
			.WithColor(BM::ColorDef::Blue).WithOutline({ .Color = BM::ColorDef::Red, .Thickness = 2.f })
			.WithRectShape({ .Size = cUISize, .Corner = 10.f })
			//.WithCircleShape(cUISize.X / 2.f).WithScale(BM::Vec2f(1.f, 0.5f))
			.OnClick(onButtonClick)
			.WithWidgetColor(0.5f, 1.f)
			.Build(m_Scene);
		BM::TextBuilder().WithParent(m_Button).AtNormalizedEntity(m_Button).WithScaleCancelEntity(m_Button)
			.WithOrigin(BM::Vec2f(0.5f)).AtZ(1.f)
			.WithColor(BM::ColorDef::Green).WithOutline({ .Color = BM::ColorDef::Black, .Thickness = 1.f })
			.WithFont(m_MainFontPtr).WithText("Hello World!")
			.Build(m_Scene);

		BM::TextBuilder().WithParent(m_Button).At(BM::Vec2f(0.f, 100.f)).WithOrigin(BM::Vec2f(0.5f))
			.WithFont(m_MainFontPtr).WithText("Attached")
			.Build(m_Scene);
	}

	{
		auto onEllipseButtonClick = [&](BM::Entity entity, auto event) {
			static size_t sPressedCount = 0;
			sPressedCount++;
			BM_INFO("Test Button pressed {} times", sPressedCount);

			entity.Patch<Comp::Transform>([&](auto& transform) { transform.Local.State.Rotation += 10.f; });

			return false;
			};

		BM::Entity ellipseButton = BM::ButtonBuilder().At({ cWindowSize.Center().X, cWindowSize.Y - 200.f })
			.WithScale({ 3.f, 2.f }).WithOrigin({ 0.25f, 0.5f }).WithRotation(90.f).AtZ(10.f)
			.WithColor(BM::ColorDef::Cyan).WithOutline({ .Color = BM::ColorDef::Yellow, .Thickness = 1.f })
			.WithCircleShape(40.f)
			.OnClick(onEllipseButtonClick)
			.Build(m_Scene);
		BM::TextBuilder().WithParent(ellipseButton).AtNormalizedEntity(ellipseButton)
			.WithScaleCancelEntity(ellipseButton, BM::Vec2f(1.25f)).WithOrigin(BM::Vec2f(0.5f)).AtZ(1.f)
			.WithColor(BM::ColorDef::White).WithOutline({ .Color = BM::ColorDef::Black, .Thickness = 1.f })
			.WithFont(m_MainFontPtr).WithText("PRESS ME\n(now!)")
			.Build(m_Scene);
	}

	{
		constexpr float cSpaceAxisX = 25.f;
		constexpr float cInputX = cUISize.X + cSpaceAxisX;

		BM::InputTextBuilder builder;
		builder.At(BM::Vec2f(cWindowSize.Center().X - cInputX, cWindowSize.Y / 3.f)).AtZ(10.f)
			.WithColor(BM::ColorDef::White).WithOutline({ .Color = BM::ColorDef::Black, .Thickness = 1.f })
			.WithRectShape({ .Size = cUISize, .Corner = 5.f });

		auto inputBuilder = builder.ToCopy();
		m_InputText = inputBuilder.AtZ(11.f)
			.WithWidgetColor(0.7f, 0.85f).WithPlaceholder("hello...")
			.WithTextChild(inputBuilder.DefaultTextChildBuilder()
				.WithFont(m_MainFontPtr)
				.Build(m_Scene))
			.Build(m_Scene);

		m_FocusText = inputBuilder.DefaultTextChildBuilder().WithParent(m_InputText)
			.At(BM::Vec2f(cInputX + cSpaceAxisX, cUISize.Center().Y)).WithOrigin(BM::Vec2f(0.f, 0.5f))
			.WithFont(m_MainFontPtr)
			.Build(m_Scene);

		auto pinBuilder = builder.ToCopy();
		pinBuilder.WithOrigin(BM::Vec2f(0.5f))
			.WithColor(BM::ColorDef::Magenta).WithOutline({ .Thickness = 0.f })
			.WithWidgetColor(0.75f, 1.f).WithPlaceholder("PIN...").WithPolicy(isdigit)
			.WithTextChild(pinBuilder.DefaultTextChildBuilder(BM::Vec2f(0.1f, 0.5f)).WithOrigin(BM::Vec2f(0.f, 0.5f))
				.WithColor(BM::ColorDef::Black).WithFont(m_MainFontPtr)
				.Build(m_Scene))
			.Build(m_Scene);
	}
}

bool GameLayer::OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept
{
	auto window = GetWindow().lock();
	if (!window)
		return false;

	switch (keyPressed.code)
	{
		using Key = sf::Keyboard::Key;

	case Key::Tab:
		GetApp().SetTimeScale(GetApp().GetContext().TimeScale > 1.f ? 1.f : 100.f);
		break;

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

	const BM::Color cRandomColor = BM::Color((uint32_t)BM_RANDOM(0, 0xFFFFFF) << 8u).WithAlpha(1.f);

	BM::Entity circle = BM::ButtonBuilder().At(cMouseCoords).WithOrigin(BM::Vec2f(0.5f)).AtZ(50.f)
		.WithColor(BM::ColorDef::Clear).WithOutline({ .Color = cRandomColor, .Thickness = 10.f })
		.WithCircleShape(cRadius).OnClick(onCirclePressed)
		.Build(m_Scene);

	BM::CircleBuilder().WithParent(circle).WithOrigin(BM::Vec2f(0.5f))
		.WithColor(BM::ColorDef::Red).WithRadius(5.f)
		.Build(m_Scene);

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
