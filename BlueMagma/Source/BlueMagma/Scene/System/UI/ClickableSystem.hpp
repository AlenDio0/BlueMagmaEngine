#pragma once
#include "Scene/System/ISystem.hpp"
#include "Base/EventDispatcher.hpp"

namespace BM::UI
{
	class ClickableSystem : public ISystem
	{
	public:
		virtual void OnEvent(Scene& scene, Event& event) noexcept override;
	private:
		bool OnMousePressed(const EventHandle::MouseButtonPressed& mousePressed, Scene& scene) noexcept;
	};
}
