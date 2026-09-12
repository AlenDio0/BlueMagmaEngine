#pragma once
#include <BlueMagma/Framework/Layer/AppLayer.hpp>
#include <BlueMagma/Math/Vec2.hpp>
#include <BlueMagma/Graphics/Camera2D.hpp>
#include <BlueMagma/Scene/Scene.hpp>
#include <BlueMagma/Scene/Entity.hpp>
#include <BlueMagma/Scene/Component/Render.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace Paddle
{
	namespace Component
	{
		struct Score
		{
			uint32_t Score;
			BM::Entity Text;
		};

		struct BotTag {};

		struct Bot
		{
			float TargetY = 0.f;
			float TargetOffsetY = 0.f;
		};
	}

	inline const BM::WindowContext g_WindowContext{ .InitialMode{{1280u, 720u}}, .SavePositionMemoryOnClose = false, .UsePositionMemoryOnOpen = false };

	class PaddleLayer : public BM::AppLayer
	{
	public:
		PaddleLayer() noexcept;

		virtual void OnAttachApplication() noexcept override;

		virtual void OnAttach() noexcept override;

		virtual void OnEvent(BM::Event& event) noexcept override;
		virtual void OnTick(float timeStep) noexcept override;
		virtual void OnUpdate(float deltaTime) noexcept override;
		virtual void OnRender() noexcept override;
	private:
		void InitEntities() noexcept;

		bool OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept;
		bool OnResized(const BM::EventHandle::Resized& resized) noexcept;

		BM::Entity CreatePaddle(const BM::Component::Transform::LocalSpace& transform, const BM::Component::TextRender& textRender, BM::Component::Transform::LocalSpace scoreTransform) noexcept;

		void TickBotPaddle(BM::Entity paddle, float timeStep) noexcept;

		void UpdatePlayerPaddle(BM::Entity paddle, sf::Keyboard::Key upKey, sf::Keyboard::Key downKey, float deltaTime) noexcept;
		void UpdateBotPaddle(BM::Entity paddle, float deltaTime) noexcept;
		void UpdatePaddle(BM::Entity paddle, float directionY, float deltaTime) noexcept;
		void UpdateBall(float deltaTime) noexcept;

		void AddScore(BM::Entity entity) noexcept;
		void PatchScore(BM::Entity entity, uint32_t score) noexcept;

		bool IsBot(BM::Entity entity) const noexcept;
		void ToggleBot(BM::Entity entity) noexcept;

		bool CheckPaddleCollision(BM::Entity paddle) const noexcept;
		void HandlePaddleCollision(BM::Entity paddle) noexcept;

		float PredictBallTargetY(float paddleX) const noexcept;

		float GetPaddleSpeed() const noexcept;
		float GetBallSpeed() const noexcept;

		void ResetPaddle() noexcept;
		void ResetBall() noexcept;
		void StartBall() noexcept;

		void StartNewGame() noexcept;

		void SetDesktopMode(bool desktopMode) noexcept;
		void UpdateWindowTitle() noexcept;
	private:
		const char* m_WindowTitle = "Paddle Game";
		BM::WindowContext m_WindowContext = g_WindowContext;

		size_t m_TickFromStartCounter = 0ull;

		BM::Camera2D m_MainCamera;

		const BM::Font* m_MainFontPtr = nullptr;

		BM::Scene m_Scene;

		BM::Entity m_Background;

		BM::Entity m_LeftPaddle;
		BM::Entity m_RightPaddle;

		BM::Entity m_Ball;
		BM::Vec2f m_BallVelocity{};
		float m_BallSpeedFactor = 1.f;

		BM::Entity m_BallSpeedFactorText;

		BM::Entity m_TimerText;
		uint16_t m_Timer = 0u;

		//BM::Entity m_FPSText;
		//BM::Entity m_TPSText;

		bool m_DesktopMode = false;
	};
}
