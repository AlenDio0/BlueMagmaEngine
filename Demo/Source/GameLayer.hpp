#pragma once
#include <BlueMagma/Layer/Layer.hpp>
#include <BlueMagma/Scene/Scene.hpp>
#include <BlueMagma/Scene/Entity.hpp>
#include <BlueMagma/Core/Timer.hpp>
#include <BlueMagma/Asset/SoundManager.hpp>
#include <string>

class GameLayer : public BM::Layer
{
public:
	GameLayer() noexcept;

	virtual void OnAttach() noexcept override;
	virtual void OnDetach() noexcept override;
	virtual void OnTransition() noexcept override;

	virtual void OnEvent(BM::Event& event) noexcept override;
	virtual void OnUpdate(float deltaTime) noexcept override;
	virtual void OnRender(sf::RenderTarget& target) noexcept override;
private:
	void InitExample() noexcept;

	bool OnKeyPressed(const BM::EventHandle::KeyPressed& keyPressed) noexcept;
	bool OnMouseMoved(const BM::EventHandle::MouseMoved& mouseMoved) noexcept;

	std::string FormatStatText(float deltaTime) const noexcept;
private:
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
};
