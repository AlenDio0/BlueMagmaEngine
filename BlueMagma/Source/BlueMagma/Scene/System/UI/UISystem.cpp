#include "bmpch.hpp"
#include "UISystem.hpp"
#include "Scene/Scene.hpp"

namespace BM
{
	using namespace Component;

	void UISystem::OnAttach(Scene& scene) noexcept
	{
		m_WidgetSystem.OnAttach(scene);
		m_WidgetColorSystem.OnAttach(scene);
		m_InputTextSystem.OnAttach(scene);
	}

	void UISystem::OnEvent(Scene& scene, Event& event) noexcept
	{
		m_WidgetSystem.OnEvent(scene, event);
		m_ClickableSystem.OnEvent(scene, event);
		m_InputTextSystem.OnEvent(scene, event);
	}

	void UISystem::OnUpdate(Scene& scene, float deltaTime) noexcept
	{
		m_WidgetSystem.OnUpdate(scene, deltaTime);
		m_WidgetColorSystem.OnUpdate(scene, deltaTime);
		m_InputTextSystem.OnUpdate(scene, deltaTime);
	}
}
