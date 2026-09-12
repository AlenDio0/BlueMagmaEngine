#include "PaddleLayer.hpp"
#include "GameLayer.hpp"
#include <BlueMagma/Scene/System/Core/TransformSystem.hpp>
#include <BlueMagma/Scene/System/Render/RenderSystem.hpp>
#include <BlueMagma/Math/Color.hpp>
#include <BlueMagma/Core/Utils.hpp>
#include <BlueMagma/Core/Random.hpp>
#include <SFML/System/Angle.hpp>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <ranges>

namespace Paddle
{
	PaddleLayer::PaddleLayer() noexcept
	{
		m_Scene.AddSystem<BM::TransformSystem>(100);
		m_Scene.AddSystem<BM::RenderSystem>();
	}

	void PaddleLayer::OnAttachApplication() noexcept
	{
		m_Scene.AttachRenderer(GetRenderer());

		m_MainFontPtr = &GetAsset<BM::Font>("Minecraft Nearest");
	}

	void PaddleLayer::OnAttach() noexcept
	{
		SetDesktopMode(m_DesktopMode);
		UpdateWindowTitle();

		InitEntities();

		ResetPaddle();
		ResetBall();
	}

	void PaddleLayer::OnEvent(BM::Event& event) noexcept
	{
		m_Scene.OnEvent(event);

		BM::EventDispatcher dispatcher(event);

		dispatcher.Dispatch<BM::EventHandle::KeyPressed>(BM_EVENT_FN(OnKeyPressed));
		dispatcher.Dispatch<BM::EventHandle::Resized>(BM_EVENT_FN(OnResized));
	}

	void PaddleLayer::OnTick(float timeStep) noexcept
	{
		if (m_BallVelocity.X != 0.f)
		{
			m_TickFromStartCounter++;

			if (IsBot(m_LeftPaddle))
				TickBotPaddle(m_LeftPaddle, timeStep);
			if (IsBot(m_RightPaddle))
				TickBotPaddle(m_RightPaddle, timeStep);

			if (m_TickFromStartCounter % 20ull == 0ull)
				m_BallSpeedFactor += 0.005f;

			if (m_TickFromStartCounter % GetApp().GetContext().TPSLimit == 0ull)
			{
				m_Timer++;
				m_TimerText.Patch<BM::Component::TextRender>([&](auto& textRender) {
					textRender.Text = std::to_string(m_Timer);
					});
			}
		}

		if (m_BallSpeedFactor > 1.f)
		{
			m_BallSpeedFactorText.Patch<BM::Component::TextRender>([&](auto& textRender) {
				textRender.Text = std::format("{:.3f}", m_BallSpeedFactor);
				});
			const float cSpeedProgress = BM::Utils::InverseLerp(m_BallSpeedFactor, 1.f, 3.f);
			BM::Color cSpeedProgressColor = BM::Utils::Lerp(BM::ColorDef::Green, BM::ColorDef::Red, cSpeedProgress);
			m_BallSpeedFactorText.Patch<BM::Component::ColorMaterial>([&](auto& material) {
				material.Color = cSpeedProgressColor;
				});
		}
	}

	void PaddleLayer::OnUpdate(float deltaTime) noexcept
	{
		m_Scene.OnUpdate(deltaTime);

		if (m_BallVelocity.X != 0.f)
		{
			using Key = sf::Keyboard::Key;
			if (!IsBot(m_LeftPaddle))
				UpdatePlayerPaddle(m_LeftPaddle, Key::W, Key::S, deltaTime);
			else
				UpdateBotPaddle(m_LeftPaddle, deltaTime);

			if (!IsBot(m_RightPaddle))
				UpdatePlayerPaddle(m_RightPaddle, Key::Up, Key::Down, deltaTime);
			else
				UpdateBotPaddle(m_RightPaddle, deltaTime);
		}

		UpdateBall(deltaTime);
	}

	void PaddleLayer::OnRender() noexcept
	{
		auto renderer = GetRenderer().lock();
		if (!renderer)
			return;

		renderer->SetCamera(m_MainCamera);
		m_Scene.OnRender();
	}

	void PaddleLayer::InitEntities() noexcept
	{
		if (auto window = GetWindow().lock())
			m_MainCamera = BM::Camera2D(window->GetSize());

		m_Scene.ClearEntities();

		const BM::Vec2f cBackgroundSize = m_MainCamera.GetSize();
		const BM::Vec2f cBackgroundCenter = cBackgroundSize.Center();

		m_Background = m_Scene.CreateEntity();
		m_Background.Add<BM::Component::RectShape>(cBackgroundSize);
		m_Background.Add<BM::Component::ColorMaterial>(BM::ColorDef::White.WithAlpha(0.1f));

		BM::Entity paddleNet = m_Scene.CreateEntityWithParent(m_Background, { .State{.Position = cBackgroundCenter, .Origin{0.5f, 0.5f}} });
		paddleNet.Add<BM::Component::RectShape>(BM::Vec2f(2.f, cBackgroundSize.Y));
		paddleNet.Add<BM::Component::ColorMaterial>(BM::ColorDef::White.WithAlpha(0.5f));

		m_Ball = m_Scene.CreateEntityWithParent(m_Background, { .State{.Origin{ 0.5f, 0.5f }} });
		m_Ball.Add<BM::Component::CircleShape>(cBackgroundSize.Y / 48.f);
		m_Ball.Add<BM::Component::ColorMaterial>(BM::ColorDef::White);

		const uint32_t cCharacterSize = (uint32_t)(cBackgroundSize.Y / 20.f);
		const float cPaddingX = cBackgroundSize.X / 90.f;
		const float cPaddingY = cBackgroundSize.Y / 32.f;
		const float cSpaceX = cBackgroundSize.X / 32.f;

		const BM::Component::TextRender cTextRender{ .FontPtr = m_MainFontPtr, .Text = "0", .CharSize = cCharacterSize };

		m_LeftPaddle = CreatePaddle({}, cTextRender, { .State{.Position{cPaddingX * 3.f, cPaddingY }} });
		m_RightPaddle = CreatePaddle({ .State{.Origin{1.f, 0.f}} }, cTextRender, { .State{.Position{cBackgroundSize.X - (cPaddingX * 3.f), cPaddingY }, .Origin{1.f, 0.f}} });

		m_BallSpeedFactorText = m_Scene.CreateEntityWithParent(m_Background, { .State{.Position{ cBackgroundCenter.X - cSpaceX, cPaddingY }, .Origin{ 1.f, 0.f }} });
		m_BallSpeedFactorText.Add<BM::Component::TextRender>(cTextRender);
		m_BallSpeedFactorText.Add<BM::Component::ColorMaterial>(BM::ColorDef::Green);

		m_TimerText = m_Scene.CreateEntityWithParent(m_Background, { .State{.Position{ cBackgroundCenter.X + cSpaceX, cPaddingY }} });
		m_TimerText.Add<BM::Component::TextRender>(cTextRender);
	}

	bool PaddleLayer::OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept
	{
		switch (keyPressed.code)
		{
			using Key = sf::Keyboard::Key;

		case Key::Add:
			GetApp().SetTimeScale(GetApp().GetContext().TimeScale + 1.f);
			break;
		case Key::Subtract:
			GetApp().SetTimeScale(std::max(0.f, GetApp().GetContext().TimeScale - 1.f));
			break;

		case Key::P:
			QueueTransitionTo<GameLayer>();
			break;
		case Key::N:
			QueueTransitionTo<PaddleLayer>();
			break;

		case Key::L:
			ToggleBot(m_LeftPaddle);
			StartNewGame();
			break;
		case Key::R:
			ToggleBot(m_RightPaddle);
			StartNewGame();
			break;
		case Key::Enter:
			StartNewGame();
			break;
		case Key::W:
		case Key::S:
		case Key::Up:
		case Key::Down:
			StartBall();
			break;

		case Key::F:
			m_DesktopMode = !m_DesktopMode;
			SetDesktopMode(m_DesktopMode);
			break;

		default:
			break;
		}

		return false;
	}

	bool PaddleLayer::OnResized(const BM::EventHandle::Resized& resized) noexcept
	{
		m_WindowContext.InitialMode.Size = resized.size;

		return false;
	}

	BM::Entity PaddleLayer::CreatePaddle(const BM::Component::Transform::LocalSpace& transform, const BM::Component::TextRender& textRender, BM::Component::Transform::LocalSpace scoreTransform) noexcept
	{
		const float cPaddleHeight = m_MainCamera.GetSize().Y / 6.f;

		const BM::Vec2f cPaddleSize{ cPaddleHeight / 8.f, cPaddleHeight };
		constexpr float cPaddleCorner = 5.f;

		BM::Entity paddle = m_Scene.CreateEntityWithParent(m_Background, transform);
		paddle.Add<BM::Component::RectShape>(cPaddleSize, cPaddleCorner);
		paddle.Add<BM::Component::ColorMaterial>(BM::ColorDef::White);

		BM::Entity scoreText = m_Scene.CreateEntityWithParent(m_Background, scoreTransform);
		scoreText.Add<BM::Component::TextRender>(textRender);

		paddle.Add<Component::Score>(0u, scoreText);

		paddle.Add<Component::Bot>();

		return paddle;
	}

	void PaddleLayer::TickBotPaddle(BM::Entity paddle, float timeStep) noexcept
	{
		if (m_TickFromStartCounter % 15ull != 0ull)
			return;

		const bool cIsLeftPaddle = paddle.GetHandle() == m_LeftPaddle.GetHandle();
		const bool cIsBallComing = cIsLeftPaddle ? m_BallVelocity.X < 0.f : m_BallVelocity.X > 0.f;

		const BM::Vec2f cPaddlePosition = paddle.Get<BM::Component::Transform>().Local.State.Position;
		const float cPaddleHalfHeight = paddle.Get<BM::Component::RectShape>().Size.Y / 2.f;
		const float cPaddleCenterY = cPaddlePosition.Y + cPaddleHalfHeight;
		const float cCameraCenterY = m_MainCamera.GetCenter().Y;

		const float cMaxStep = GetPaddleSpeed() * timeStep;

		if (!cIsBallComing)
		{
			paddle.Patch<Component::Bot>([&](auto& bot) {
				bot.TargetY = cCameraCenterY - cPaddleHalfHeight;
				bot.TargetOffsetY = -1.f;
				});

			return;
		}

		const float cPredictedBallTargetY = PredictBallTargetY(cPaddlePosition.X);
		paddle.Patch<Component::Bot>([&](auto& bot) {
			if (bot.TargetOffsetY == -1.f)
				bot.TargetOffsetY = (float)BM_RANDOM_DOUBLE(-0.75f, 0.75f);

			bot.TargetY = cPredictedBallTargetY - cPaddleHalfHeight - (bot.TargetOffsetY * cPaddleHalfHeight);
			});
	}

	void PaddleLayer::UpdatePlayerPaddle(BM::Entity paddle, sf::Keyboard::Key upKey, sf::Keyboard::Key downKey, float deltaTime) noexcept
	{
		if (auto window = GetWindow().lock())
		{
			if (!window->HasFocus())
				return;
		}

		namespace Keyboard = sf::Keyboard;
		float directionY = 0.f;
		if (Keyboard::isKeyPressed(upKey))
			directionY -= 1.f;
		if (Keyboard::isKeyPressed(downKey))
			directionY += 1.f;

		UpdatePaddle(paddle, directionY, deltaTime);
	}

	void PaddleLayer::UpdateBotPaddle(BM::Entity paddle, float deltaTime) noexcept
	{
		const float cPaddleY = paddle.Get<BM::Component::Transform>().Local.State.Position.Y;
		const float cDistance = paddle.Get<Component::Bot>().TargetY - cPaddleY;
		const float cMaxStep = GetPaddleSpeed() * deltaTime;

		const float cDirectionY = cMaxStep == 0.f ? 0.f : std::clamp(cDistance / cMaxStep, -1.f, 1.f);
		UpdatePaddle(paddle, cDirectionY, deltaTime);
	}

	void PaddleLayer::UpdatePaddle(BM::Entity paddle, float directionY, float deltaTime) noexcept
	{
		if (directionY == 0.f)
			return;

		const float cMovementY = GetPaddleSpeed() * directionY * deltaTime;

		const float cCameraHeight = m_MainCamera.GetSize().Y;
		const float cPaddingY = cCameraHeight / 75.f;
		const float cPaddleHeight = paddle.Get<BM::Component::RectShape>().Size.Y;

		paddle.Patch<BM::Component::Transform>([&](auto& transform) {
			float& positionY = transform.Local.State.Position.Y;

			positionY += cMovementY;

			if (positionY < cPaddingY)
				positionY = cPaddingY;
			else if (positionY + cPaddleHeight > cCameraHeight - cPaddingY)
				positionY = cCameraHeight - cPaddleHeight - cPaddingY;
			});
	}

	void PaddleLayer::UpdateBall(float deltaTime) noexcept
	{
		if (m_BallVelocity == 0.f)
			return;

		const BM::Vec2f cCameraSize = m_MainCamera.GetSize();
		const float cBallRadius = m_Ball.Get<BM::Component::CircleShape>().Radius;

		const float cMovementLength = m_BallVelocity.Length() * deltaTime;
		const uint32_t cTotalSteps = (uint32_t)std::max(1.f, std::ceil(cMovementLength / cBallRadius));
		const float cSubDeltaTime = deltaTime / (float)cTotalSteps;

		for (uint32_t step = 0u; step < cTotalSteps; step++)
		{
			BM::Vec2f ballPosition = m_Ball.Get<BM::Component::Transform>().Local.State.Position;
			m_Ball.Patch<BM::Component::Transform>([&](auto& transform) {
				auto& position = transform.Local.State.Position;
				position += m_BallVelocity * cSubDeltaTime;
				ballPosition = position;
				});

			const bool cHasBouncedTop = ballPosition.Y - cBallRadius < 0.f && m_BallVelocity.Y < 0.f;
			const bool cHasBouncedBottom = ballPosition.Y + cBallRadius > cCameraSize.Y && m_BallVelocity.Y > 0.f;
			if (cHasBouncedTop || cHasBouncedBottom)
				m_BallVelocity.Y *= -1.f;

			if (m_BallVelocity.X < 0.f && CheckPaddleCollision(m_LeftPaddle))
				HandlePaddleCollision(m_LeftPaddle);
			else if (m_BallVelocity.X > 0.f && CheckPaddleCollision(m_RightPaddle))
				HandlePaddleCollision(m_RightPaddle);

			bool scored = false;
			if (ballPosition.X + cBallRadius < 0.f)
			{
				AddScore(m_RightPaddle);
				scored = true;
			}
			else if (ballPosition.X - cBallRadius > cCameraSize.X)
			{
				AddScore(m_LeftPaddle);
				scored = true;
			}

			if (scored)
			{
				ResetPaddle();
				ResetBall();

				if (IsBot(m_LeftPaddle) && IsBot(m_RightPaddle))
					StartBall();

				return;
			}
		}
	}

	void PaddleLayer::AddScore(BM::Entity entity) noexcept
	{
		PatchScore(entity, entity.Get<Component::Score>().Score + 1);
	}

	void PaddleLayer::PatchScore(BM::Entity entity, uint32_t score) noexcept
	{
		entity.Patch<Component::Score>([&](Component::Score& scoreComp) {
			scoreComp.Score = score;
			scoreComp.Text.Patch<BM::Component::TextRender>([&](auto& textRender) {
				textRender.Text = std::to_string(scoreComp.Score);
				});
			});
	}

	bool PaddleLayer::IsBot(BM::Entity entity) const noexcept
	{
		return entity.HasAll<Component::BotTag>();
	}

	void PaddleLayer::ToggleBot(BM::Entity entity) noexcept
	{
		if (!IsBot(entity))
			entity.Add<Component::BotTag>();
		else
			entity.Remove<Component::BotTag>();
	}

	bool PaddleLayer::CheckPaddleCollision(BM::Entity paddle) const noexcept
	{
		const auto& cPaddleTransform = paddle.Get<BM::Component::Transform>().Local.State;
		const BM::Vec2f cPaddlePosition = cPaddleTransform.Position;
		const BM::Vec2f cPaddleOrigin = cPaddleTransform.Origin;

		const BM::Vec2f cPaddleSize = paddle.Get<BM::Component::RectShape>().Size;
		const BM::Vec2f cPaddleTopLeft = cPaddlePosition - (cPaddleSize * cPaddleOrigin);

		const BM::Vec2f cBallPosition = m_Ball.Get<BM::Component::Transform>().Local.State.Position;
		const float cBallRadius = m_Ball.Get<BM::Component::CircleShape>().Radius;

		const BM::Vec2f cNearest
		{
			std::clamp(cBallPosition.X, cPaddleTopLeft.X, cPaddleTopLeft.X + cPaddleSize.X),
			std::clamp(cBallPosition.Y, cPaddleTopLeft.Y, cPaddleTopLeft.Y + cPaddleSize.Y)
		};
		const float cSquaredDistance = cBallPosition.SquaredDistance(cNearest);

		return cSquaredDistance < cBallRadius * cBallRadius;
	}

	void PaddleLayer::HandlePaddleCollision(BM::Entity paddle) noexcept
	{
		const float cPaddleY = paddle.Get<BM::Component::Transform>().Local.State.Position.Y;
		const BM::Vec2f cPaddleSize = paddle.Get<BM::Component::RectShape>().Size;

		const float cBallY = m_Ball.Get<BM::Component::Transform>().Local.State.Position.Y;
		const float cBallRadius = m_Ball.Get<BM::Component::CircleShape>().Radius;

		constexpr float cMaxBounceAngle = sf::degrees(60.f).asRadians();
		const float cRelativeDistanceFromCenterY = cBallY - (cPaddleY + (cPaddleSize.Y / 2.f));
		const float cNormalizedDistanceFromCenterY = std::clamp(cRelativeDistanceFromCenterY / (cPaddleSize.Y / 2.f), -1.f, 1.f);

		const float cBounceAngle = cNormalizedDistanceFromCenterY * cMaxBounceAngle;
		const float cBallDirectionX = m_BallVelocity.X < 0.f ? 1.f : -1.f;
		m_BallVelocity = BM::Vec2f(std::cos(cBounceAngle) * cBallDirectionX, std::sin(cBounceAngle)) * GetBallSpeed() * m_BallSpeedFactor;
	}

	float PaddleLayer::PredictBallTargetY(float paddleX) const noexcept
	{
		const BM::Vec2f cBallPosition = m_Ball.Get<BM::Component::Transform>().Local.State.Position;
		const float cCameraCenterY = m_MainCamera.GetCenter().Y;
		const float cCameraHeight = m_MainCamera.GetSize().Y;

		if (m_BallVelocity.X == 0.f)
			return cCameraCenterY;

		const float cTimeToReachPaddle = (paddleX - cBallPosition.X) / m_BallVelocity.X;
		if (cTimeToReachPaddle < 0.f)
			return cCameraCenterY;

		float predictedY = std::abs(cBallPosition.Y + (m_BallVelocity.Y * cTimeToReachPaddle));

		const uint8_t cMaxPrediction = -1;
		for (size_t prediction = 0; predictedY > cCameraHeight && prediction < cMaxPrediction; prediction++)
			predictedY = std::abs((2.f * cCameraHeight) - predictedY);

		return predictedY;
	}

	float PaddleLayer::GetPaddleSpeed() const noexcept
	{
		return m_MainCamera.GetSize().Y / 2.5f;
	}

	float PaddleLayer::GetBallSpeed() const noexcept
	{
		return m_MainCamera.GetSize().X / 3.f;
	}

	void PaddleLayer::ResetPaddle() noexcept
	{
		const BM::Vec2f cCameraSize = m_MainCamera.GetSize();
		const BM::Vec2f cCameraCenter = m_MainCamera.GetCenter();
		const float cPaddleHalfHeight = m_LeftPaddle.Get<BM::Component::RectShape>().Size.Y / 2.f;
		const float cPaddleStartY = cCameraCenter.Y - cPaddleHalfHeight;

		const float cPaddingX = cCameraSize.X / 90.f;
		const float cPaddingY = cCameraSize.Y / 32.f;

		m_LeftPaddle.Patch<BM::Component::Transform>([&](auto& transform) {
			transform.Local.State.Position = BM::Vec2f(cPaddingX, cPaddleStartY);
			});
		m_RightPaddle.Patch<BM::Component::Transform>([&](auto& transform) {
			transform.Local.State.Position = BM::Vec2f(cCameraSize.X - cPaddingX, cPaddleStartY);
			});

		m_LeftPaddle.Patch<Component::Bot>([&](auto& bot) {
			bot.TargetY = cPaddleStartY;
			});
		m_RightPaddle.Patch<Component::Bot>([&](auto& bot) {
			bot.TargetY = cPaddleStartY;
			});
	}

	void PaddleLayer::ResetBall() noexcept
	{
		m_Ball.Patch<BM::Component::Transform>([&](auto& transform) {
			transform.Local.State.Position = m_MainCamera.GetCenter();
			});

		m_BallVelocity = BM::Vec2f(0.f);
		m_BallSpeedFactor = 1.f;
	}

	void PaddleLayer::StartBall() noexcept
	{
		if (m_BallVelocity != 0.f)
			return;

		const float cRandomAngle = sf::degrees((float)BM_RANDOM_DOUBLE(-40.f, 40.f)).asRadians();
		const float cDirectionX = BM_RANDOM(0, 1) % 2 ? -1.f : 1.f;
		m_BallVelocity = BM::Vec2f(std::cos(cRandomAngle) * cDirectionX, std::sin(cRandomAngle)) * GetBallSpeed();

		m_Timer = 0u;
	}

	void PaddleLayer::StartNewGame() noexcept
	{
		m_TickFromStartCounter = 0ull;
		ResetPaddle();
		ResetBall();

		PatchScore(m_LeftPaddle, 0u);
		PatchScore(m_RightPaddle, 0u);

		if (IsBot(m_LeftPaddle) && IsBot(m_RightPaddle))
			StartBall();

		UpdateWindowTitle();
	}

	void PaddleLayer::SetDesktopMode(bool desktopMode) noexcept
	{
		BM::WindowContext windowContext = m_WindowContext;

		if (desktopMode)
		{
			windowContext.InitialMode = BM::WindowMode::GetDesktopMode();
			windowContext.InitialStyle = BM::WindowStyle::None;

			windowContext.UsePositionMemoryOnOpen = false;
			windowContext.SavePositionMemoryOnClose = true;
		}

		GetApp().CreateOrReplaceWindow(windowContext);
	}

	void PaddleLayer::UpdateWindowTitle() noexcept
	{
		auto window = GetWindow().lock();
		if (!window)
			return;

		std::string title = std::format("{} ~ ", m_WindowTitle);

		struct BotLabel {
			bool Enabled = false;
			std::string_view Label;
		};

		if (m_LeftPaddle.IsValid() && m_RightPaddle.IsValid())
		{
			const BotLabel sBots[]{ { IsBot(m_LeftPaddle), "LBot" }, { IsBot(m_RightPaddle), "RBot" } };
			auto botsView = sBots
				| std::views::filter([](const BotLabel& botLabel) { return botLabel.Enabled; })
				| std::views::transform([](const BotLabel& bot) { return bot.Label; });

			title += BM::Utils::JoinWith(botsView, " | ");
		}

		window->SetTitle(title);
		m_WindowContext.Title = title;
	}
}
