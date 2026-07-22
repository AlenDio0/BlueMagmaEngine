#include "PaddleLayer.hpp"
#include <BlueMagma/Scene/System/TransformSystem.hpp>
#include <BlueMagma/Scene/System/RenderSystem.hpp>
#include <BlueMagma/Core/Random.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Angle.hpp>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <cstdint>

PaddleLayer::PaddleLayer() noexcept
	: m_MainCamera(GetWindow().GetSize())
{
	m_Scene.AttachRenderer(GetRenderer());

	m_Scene.AddSystem<BM::TransformSystem>(100);
	m_Scene.AddSystem<BM::RenderSystem>();
}

void PaddleLayer::OnAttach() noexcept
{
	GetWindow().SetFPSLimit(180u);
	GetApp().Context.TPSLimit = 60u;

	const BM::Vec2f cCameraSize = m_MainCamera.GetSize();
	const BM::Vec2f cCameraCenter = m_MainCamera.GetCenter();
	const float cPaddingX = cCameraSize.X / 90.f;

	m_LeftPaddle = CreatePaddle({ .State{.Position{cPaddingX, cCameraCenter.Y}} });
	m_RightPaddle = CreatePaddle({ .State{.Position{cCameraSize.X - cPaddingX, cCameraCenter.Y}, .Origin{1.f, 0.f}} });

	m_Ball = m_Scene.CreateEntity({ .State{.Position = cCameraCenter, .Origin{0.5f, 0.5f} }, .Z = 1.f });
	m_Ball.Add<BM::Component::CircleShape>(cCameraSize.Y / 48.f);
	m_Ball.Add<BM::Component::ColorMaterial>(sf::Color::White);

	BM::Entity background = m_Scene.CreateEntity({ .Z = -2.f });
	background.Add<BM::Component::RectShape>(cCameraSize);
	background.Add<BM::Component::ColorMaterial>(sf::Color(0xFFFFFF10));

	BM::Entity axis = m_Scene.CreateEntity({ .State{.Position = cCameraCenter, .Origin{0.5f, 0.5f}}, .Z = -1.f });
	axis.Add<BM::Component::RectShape>(BM::Vec2f(2.f, cCameraSize.Y));
	axis.Add<BM::Component::ColorMaterial>(sf::Color(0xFFFFFF80));

	const BM::Font& cFont = GetAsset<BM::Font>("Minecraft");
	const float cPaddingY = cCameraSize.Y / 32.f;
	const float cSpaceX = cCameraSize.X / 32.f;
	const uint32_t cCharacterSize = (uint32_t)(cCameraSize.Y / 20.f);

	m_BallSpeedFactorText = m_Scene.CreateEntity({ .State{.Position{cCameraCenter.X - cSpaceX, cPaddingY }, .Origin{1.f, 0.f}} });
	m_BallSpeedFactorText.Add<BM::Component::TextRender>(&cFont, std::format("{:.3f}", m_BallSpeedFactor), cCharacterSize);
	m_BallSpeedFactorText.Add<BM::Component::ColorMaterial>(sf::Color::Green);

	m_TimerText = m_Scene.CreateEntity({ .State{.Position{cCameraCenter.X + cSpaceX, cPaddingY }} });
	m_TimerText.Add<BM::Component::TextRender>(&cFont, "0", cCharacterSize);

	m_LeftScoreText = m_Scene.CreateEntityWithParent(axis, { .State{.Position{-cSpaceX, 0.f }, .Origin{1.f, 0.5f}} });
	m_LeftScoreText.Add<BM::Component::TextRender>(&cFont, "0", cCharacterSize);

	m_RightScoreText = m_Scene.CreateEntityWithParent(axis, { .State{.Position{cSpaceX, 0.f }, .Origin{0.f, 0.5f}} });
	m_RightScoreText.Add<BM::Component::TextRender>(&cFont, "0", cCharacterSize);
}

void PaddleLayer::OnEvent(BM::Event& event) noexcept
{
	m_Scene.OnEvent(event);

	BM::EventDispatcher dispatcher(event);

	dispatcher.Dispatch<BM::EventHandle::KeyPressed>(BM_EVENT_FN(OnKeyPressed));
}

void PaddleLayer::OnTick(float timeStep) noexcept
{
	m_LeftScoreText.Patch<BM::Component::TextRender>([&](auto& textRender) {
		textRender.Text = std::to_string(m_LeftScore);
		});
	m_RightScoreText.Patch<BM::Component::TextRender>([&](auto& textRender) {
		textRender.Text = std::to_string(m_RightScore);
		});

	if (m_BallVelocity.X != 0.f)
	{
		if (m_IsLeftBot)
			TickBotPaddle(m_LeftPaddle, timeStep);
		if (m_IsRightBot)
			TickBotPaddle(m_RightPaddle, timeStep);

		m_BallTickCounter++;
		if (m_BallTickCounter >= 20ull)
		{
			m_BallTickCounter = 0ull;
			m_BallSpeedFactor += 0.005f;
		}

		m_TimerTickCounter++;
		if (m_TimerTickCounter > (size_t)std::ceil(1.f / timeStep))
		{
			m_TimerTickCounter = 0ull;
			m_Timer++;
			m_TimerText.Patch<BM::Component::TextRender>([&](auto& textRender) {
				textRender.Text = std::to_string(m_Timer);
				});
		}
	}

	m_BallSpeedFactorText.Patch<BM::Component::TextRender>([&](auto& textRender) {
		textRender.Text = std::format("{:.3f}", m_BallSpeedFactor);
		});
	m_BallSpeedFactorText.Patch<BM::Component::ColorMaterial>([&](auto& material) {
		const float cSpeedColor = std::clamp((m_BallSpeedFactor - 1.f) / 3.f, 0.f, 1.f);
		material.Color.r = (uint8_t)std::lerp(0.f, 255.f, cSpeedColor);
		material.Color.g = (uint8_t)std::lerp(255.f, 0.f, cSpeedColor);
		material.Color.b = 0u;
		});

}

void PaddleLayer::OnUpdate(float deltaTime) noexcept
{
	m_Scene.OnUpdate(deltaTime);

	if (m_BallVelocity.X != 0.f)
	{
		using Key = sf::Keyboard::Key;
		if (!m_IsLeftBot)
			UpdatePlayerPaddle(m_LeftPaddle, Key::W, Key::S, deltaTime);
		else
			UpdateBotPaddle(m_LeftPaddle, m_LeftBotTargetY, deltaTime);

		if (!m_IsRightBot)
			UpdatePlayerPaddle(m_RightPaddle, Key::Up, Key::Down, deltaTime);
		else
			UpdateBotPaddle(m_RightPaddle, m_RightBotTargetY, deltaTime);
	}

	UpdateBall(deltaTime);
}

void PaddleLayer::OnRender() noexcept
{
	GetRenderer().SetCamera(m_MainCamera);
	m_Scene.OnRender();
}

BM::Entity PaddleLayer::CreatePaddle(BM::Component::Transform::LocalSpace transform) noexcept
{
	const float cPaddleHeight = m_MainCamera.GetSize().Y / 6.f;

	const BM::Vec2f cPaddleSize{ cPaddleHeight / 8.f, cPaddleHeight };
	constexpr float cPaddleCorner = 1.f;

	transform.State.Position.Y -= cPaddleHeight / 2.f;

	BM::Entity entity = m_Scene.CreateEntity(transform);
	entity.Add<BM::Component::RectShape>(cPaddleSize, cPaddleCorner);
	entity.Add<BM::Component::ColorMaterial>(sf::Color::White);

	return entity;
}

bool PaddleLayer::OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept
{
	switch (keyPressed.code)
	{
		using Key = sf::Keyboard::Key;

	case Key::Add:
		GetApp().Context.TimeScale += 1.f;
		break;
	case Key::Subtract:
		GetApp().Context.TimeScale = std::max(0.f, GetApp().Context.TimeScale - 1.f);
		break;

	case Key::N:
		QueueTransitionTo<PaddleLayer>();
		break;

	case Key::L:
		m_IsLeftBot = !m_IsLeftBot;
		StartNewGame();
		break;
	case Key::R:
		m_IsRightBot = !m_IsRightBot;
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
	}

	return false;
}

void PaddleLayer::TickBotPaddle(BM::Entity paddle, float timeStep) noexcept
{
	const bool cIsLeftPaddle = paddle.GetHandle() == m_LeftPaddle.GetHandle();
	const bool cIsBallComing = cIsLeftPaddle ? m_BallVelocity.X < 0.f : m_BallVelocity.X > 0.f;

	float& botTargetY = cIsLeftPaddle ? m_LeftBotTargetY : m_RightBotTargetY;
	float& botTargetOffsetY = cIsLeftPaddle ? m_LeftBotTargetOffsetY : m_RightBotTargetOffsetY;

	const BM::Vec2f cPaddlePosition = paddle.Get<BM::Component::Transform>().Local.State.Position;
	const float cPaddleHalfHeight = paddle.Get<BM::Component::RectShape>().Size.Y / 2.f;
	const float cPaddleCenterY = cPaddlePosition.Y + cPaddleHalfHeight;
	const float cCameraCenterY = m_MainCamera.GetCenter().Y;

	const float cMaxStep = CalculatePaddleSpeed() * timeStep;

	if (!cIsBallComing)
	{
		botTargetY = cCameraCenterY - cPaddleHalfHeight;
		botTargetOffsetY = -1.f;

		return;
	}

	if (botTargetOffsetY == -1.f)
		botTargetOffsetY = BM_RANDOM_DOUBLE(-0.75f, 0.75f);

	const float cPredictedBallTargetY = PredictBallTargetY(cPaddlePosition.X);
	botTargetY = cPredictedBallTargetY - cPaddleHalfHeight - (botTargetOffsetY * cPaddleHalfHeight);
}

void PaddleLayer::UpdatePlayerPaddle(BM::Entity paddle, sf::Keyboard::Key upKey, sf::Keyboard::Key downKey, float deltaTime) noexcept
{
	if (!GetWindow().HasFocus())
		return;

	namespace Keyboard = sf::Keyboard;
	float directionY = 0.f;
	if (Keyboard::isKeyPressed(upKey))
		directionY -= 1.f;
	if (Keyboard::isKeyPressed(downKey))
		directionY += 1.f;

	UpdatePaddle(paddle, directionY, deltaTime);
}

void PaddleLayer::UpdateBotPaddle(BM::Entity paddle, float targetY, float deltaTime) noexcept
{
	const float cPaddleY = paddle.Get<BM::Component::Transform>().Local.State.Position.Y;
	const float cDistance = targetY - cPaddleY;
	const float cMaxStep = CalculatePaddleSpeed() * deltaTime;

	const float cDirectionY = cMaxStep == 0.f ? 0.f : std::clamp(cDistance / cMaxStep, -1.f, 1.f);
	UpdatePaddle(paddle, cDirectionY, deltaTime);
}

void PaddleLayer::UpdatePaddle(BM::Entity paddle, float directionY, float deltaTime) noexcept
{
	if (directionY == 0.f)
		return;

	const float cMovementY = CalculatePaddleSpeed() * directionY * deltaTime;

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
			m_RightScore++;
			scored = true;
		}
		else if (ballPosition.X - cBallRadius > cCameraSize.X)
		{
			m_LeftScore++;
			scored = true;
		}

		if (scored)
		{
			ResetPaddle();
			ResetBall();

			if (m_IsLeftBot && m_IsRightBot)
				StartBall();

			return;
		}
	}
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
	m_BallVelocity = BM::Vec2f(std::cos(cBounceAngle) * cBallDirectionX, std::sin(cBounceAngle)) * CalculateBallSpeed() * m_BallSpeedFactor;
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
	while (predictedY > cCameraHeight)
		predictedY = std::abs((2.f * cCameraHeight) - predictedY);

	return predictedY;
}

float PaddleLayer::CalculatePaddleSpeed() const noexcept
{
	return m_MainCamera.GetSize().Y / 2.5f;
}

float PaddleLayer::CalculateBallSpeed() const noexcept
{
	return m_MainCamera.GetSize().X / 3.f;
}

void PaddleLayer::ResetPaddle() noexcept
{
	const float cCameraCenterY = m_MainCamera.GetCenter().Y;
	const float cPaddleHalfHeight = m_LeftPaddle.Get<BM::Component::RectShape>().Size.Y / 2.f;
	const float cPaddleStartY = cCameraCenterY - cPaddleHalfHeight;

	m_LeftPaddle.Patch<BM::Component::Transform>([&](auto& transform) {
		transform.Local.State.Position.Y = cPaddleStartY;
		});
	m_RightPaddle.Patch<BM::Component::Transform>([&](auto& transform) {
		transform.Local.State.Position.Y = cPaddleStartY;
		});

	m_LeftBotTargetY = cPaddleStartY;
	m_RightBotTargetY = cPaddleStartY;
}

void PaddleLayer::ResetBall() noexcept
{
	m_Ball.Patch<BM::Component::Transform>([&](auto& transform) {
		transform.Local.State.Position = m_MainCamera.GetCenter();
		});

	m_BallVelocity = BM::Vec2f(0.f);
	m_BallSpeedFactor = 1.f;
	m_BallTickCounter = 0;
}

void PaddleLayer::StartBall() noexcept
{
	if (m_BallVelocity != 0.f)
		return;

	const float cRandomAngle = sf::degrees((float)BM_RANDOM_DOUBLE(-40.f, 40.f)).asRadians();
	const float cDirectionX = BM_RANDOM(0, 1) % 2 ? -1.f : 1.f;
	m_BallVelocity = BM::Vec2f(std::cos(cRandomAngle) * cDirectionX, std::sin(cRandomAngle)) * CalculateBallSpeed();

	m_Timer = 0u;
}

void PaddleLayer::StartNewGame() noexcept
{
	ResetPaddle();
	ResetBall();

	m_LeftScore = 0u;
	m_RightScore = 0u;

	if (m_IsLeftBot && m_IsRightBot)
		StartBall();
}
