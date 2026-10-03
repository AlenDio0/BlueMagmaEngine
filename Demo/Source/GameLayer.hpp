#pragma once
#include <BlueMagma/Framework/Layer/AppLayer.hpp>
#include <BlueMagma/Core/Timer.hpp>
#include <BlueMagma/Asset/SoundManager.hpp>
#include <BlueMagma/Graphics/Camera2D.hpp>
#include <BlueMagma/Input/InputManager.hpp>
#include <BlueMagma/Scene/Scene.hpp>
#include <BlueMagma/Scene/Entity.hpp>
#include <string>

class GameLayer : public BM::AppLayer
{
public:
	GameLayer(bool initExample = false, bool initUIExample = true) noexcept;

	virtual void OnAttachApplication() noexcept override;

	virtual void OnAttach() noexcept override;
	virtual void OnDetach() noexcept override;
	virtual void OnTransition() noexcept override;

	virtual void OnEvent(BM::Event& event) noexcept override;
	virtual void OnUpdate(float deltaTime) noexcept override;
	virtual void OnRender() noexcept override;
private:
	void InitExample() noexcept;
	void InitUIExample() noexcept;
private:
	void CreateCircle(BM::Vec2i position) noexcept;
private:
	bool OnMouseMoved(const BM::EventHandle::MouseMoved& mouseMoved) noexcept;
	bool OnMouseScrolled(const BM::EventHandle::MouseWheelScrolled& mouseScrolled) noexcept;

	void UpdateMouseCursor() noexcept;
	void UpdateMouseRender(BM::Vec2i point) noexcept;
	std::string FormatStatText(float deltaTime) const noexcept;
private:
	BM::Camera2D* m_ActiveCameraPtr;
	BM::Camera2D m_MainCamera;
	BM::Camera2D m_ButtonCamera;

	const BM::Font* m_MainFontPtr = nullptr;

	BM::InputManager m_Input;

	BM::Scene m_Scene;

	BM::Entity m_StatText;
	BM::Timer m_FPSTimer;
	uint32_t m_FPSCounter;

	bool m_InitExample;
	bool m_InitUIExample;

	BM::Entity m_Button;
	BM::Entity m_InputText;
	BM::Entity m_FocusText;

	size_t m_ButtonRotationCount = 0ull;

	BM::Entity m_MouseRender;

	BM::SoundManager m_SoundManager;

	float m_ButtonSpeedFactor;
};
