#pragma once
#include <BlueMagma/Layer/AppLayer.hpp>
#include <BlueMagma/Core/Camera2D.hpp>
#include <BlueMagma/Scene/Scene.hpp>
#include <BlueMagma/Scene/Component/Base.hpp>
#include <BlueMagma/Scene/Entity.hpp>
#include <BlueMagma/Core/Vec2.hpp>
#include <SFML/Window/Keyboard.hpp>

class PaddleLayer : public BM::AppLayer
{
public:
	PaddleLayer() noexcept;

	virtual void OnAttach() noexcept override;

	virtual void OnEvent(BM::Event& event) noexcept override;
	virtual void OnTick(float timeStep) noexcept override;
	virtual void OnUpdate(float deltaTime) noexcept override;
	virtual void OnRender() noexcept override;
private:
	BM::Entity CreatePaddle(BM::Component::Transform::LocalSpace transform) noexcept;

	bool OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept;

	void TickBotPaddle(BM::Entity paddle, float timeStep) noexcept;

	void UpdatePlayerPaddle(BM::Entity paddle, sf::Keyboard::Key upKey, sf::Keyboard::Key downKey, float deltaTime) noexcept;
	void UpdateBotPaddle(BM::Entity paddle, float targetY, float deltaTime) noexcept;
	void UpdatePaddle(BM::Entity paddle, float directionY, float deltaTime) noexcept;
	void UpdateBall(float deltaTime) noexcept;

	bool CheckPaddleCollision(BM::Entity paddle) const noexcept;
	void HandlePaddleCollision(BM::Entity paddle) noexcept;

	float PredictBallTargetY(float paddleX) const noexcept;

	float CalculatePaddleSpeed() const noexcept;
	float CalculateBallSpeed() const noexcept;

	void ResetPaddle() noexcept;
	void ResetBall() noexcept;
	void StartBall() noexcept;

	void StartNewGame() noexcept;
private:
	BM::Camera2D m_MainCamera;

	BM::Scene m_Scene;

	BM::Entity m_LeftPaddle;
	BM::Entity m_RightPaddle;

	bool m_IsLeftBot = false;
	bool m_IsRightBot = false;
	float m_LeftBotTargetY = 0.f;
	float m_RightBotTargetY = 0.f;
	float m_LeftBotTargetOffsetY = 0.f;
	float m_RightBotTargetOffsetY = 0.f;

	BM::Entity m_Ball;
	BM::Vec2f m_BallVelocity{};
	float m_BallSpeedFactor = 1.f;
	size_t m_BallTickCounter = 0ull;

	BM::Entity m_BallSpeedFactorText;

	BM::Entity m_TimerText;
	uint16_t m_Timer = 0u;
	size_t m_TimerTickCounter = 0ull;

	BM::Entity m_LeftScoreText;
	BM::Entity m_RightScoreText;
	uint16_t m_LeftScore = 0u;
	uint16_t m_RightScore = 0u;

};
