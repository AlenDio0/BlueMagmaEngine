#pragma once
#include "Scene/System/ISystem.hpp"
#include "WidgetSystem.hpp"
#include "WidgetColorSystem.hpp"
#include "ClickableSystem.hpp"
#include "InputTextSystem.hpp"

namespace BM
{
	class UISystem : public ISystem
	{
	public:
		virtual void OnAttach(Scene& scene) noexcept override;

		virtual void OnEvent(Scene& scene, Event& event) noexcept override;
		virtual void OnUpdate(Scene& scene, float deltaTime) noexcept override;
	private:
		UI::WidgetSystem m_WidgetSystem;
		UI::WidgetColorSystem m_WidgetColorSystem;
		UI::ClickableSystem m_ClickableSystem;
		UI::InputTextSystem m_InputTextSystem;
	};
}
