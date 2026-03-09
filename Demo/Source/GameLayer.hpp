#pragma once
#include <BlueMagma/Layer/AppLayer.hpp>
#include <BlueMagma/Core/Camera2D.hpp>
#include <BlueMagma/Scene/Scene.hpp>
#include <BlueMagma/Scene/Entity.hpp>
#include <BlueMagma/Core/Timer.hpp>
#include <BlueMagma/Asset/SoundManager.hpp>
#include <string>

class GameLayer : public BM::AppLayer
{
public:
	GameLayer() noexcept;

	virtual void OnAttach() noexcept override;
	virtual void OnDetach() noexcept override;
	virtual void OnTransition() noexcept override;

	virtual void OnEvent(BM::Event& event) noexcept override;
	virtual void OnUpdate(float deltaTime) noexcept override;
	virtual void OnRender() noexcept override;
private:
	void InitExample() noexcept;
	void InitUIExample() noexcept;

	bool OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept;
	bool OnMouseMoved(const BM::EventHandle::MouseMoved& mouseMoved) noexcept;
	bool OnMousePressed(const BM::EventHandle::MouseButtonPressed& mousePressed) noexcept;
	bool OnMouseScrolled(const BM::EventHandle::MouseWheelScrolled& mouseScrolled) noexcept;

	void UpdateMouseRect(BM::Vec2f position) noexcept;
	std::string FormatStatText(float deltaTime) const noexcept;
private:
	BM::Camera2D* m_ActiveCameraPtr;
	BM::Camera2D m_MainCamera;
	BM::Camera2D m_ButtonCamera;

	BM::Scene m_Scene;

	BM::Entity m_Background;
	BM::Entity m_StatText;

	BM::Entity m_Button;
	BM::Entity m_InputText;
	BM::Entity m_FocusText;

	BM::Entity m_MouseRect;

	BM::Timer m_StatTimer;
	BM::Timer m_SwitchTimer;

	BM::SoundManager m_SoundManager;

	float m_ButtonSpeedFactor;
};
