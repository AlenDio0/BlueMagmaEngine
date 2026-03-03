#pragma once
#include "Base/EventDispatcher.hpp"

namespace BM
{
	class Application;
	class LayerMachine;

	class Layer
	{
	public:
		inline Layer() noexcept = default;
		inline virtual ~Layer() noexcept = default;

		inline virtual void OnAttach() noexcept {}
		inline virtual void OnDetach() noexcept {}
		inline virtual void OnTransition() noexcept {}

		inline virtual void OnEvent(Event& event) noexcept {}
		inline virtual void OnUpdate(float deltaTime) noexcept {}
		inline virtual void OnRender() noexcept {}
	};
}
